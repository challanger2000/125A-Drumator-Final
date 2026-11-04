# Transactional repair for Steinberg's pinned VSTGUI (Windows VST3 only).
# The vendor increments IdleUpdateHandler::users BEFORE allocating the first
# CVSTGUITimer. A bad_alloc then leaves users=1 with no timer, and later
# editor opens never create one. Keep the upstream source and SDK pin intact;
# modify only the external SDK checkout being built, fail closed on drift.
function(drumator_patch_vstgui_idle_start source_file)
  if(NOT WIN32)
    return()
  endif()
  if(NOT EXISTS "${source_file}")
    message(FATAL_ERROR "Drumator: missing pinned VSTGUI editor source: ${source_file}")
  endif()
  file(READ "${source_file}" _code)
  set(_original [=[
	static void start ()
	{
		auto& instance = get ();
		if (++instance.users == 1)
		{
			instance.timer = VSTGUI::makeOwned<VSTGUI::CVSTGUITimer> (
			    [] (VSTGUI::CVSTGUITimer*) { gUpdateHandlerInit.get ()->triggerDeferedUpdates (); },
			    1000 / 30);
		}
	}
]=])
  set(_replacement [=[
	static void start ()
	{
		auto& instance = get ();
		if (++instance.users == 1)
		{
			// Drumator NV01: the first timer allocation may throw. Without
			// rollback, users remains 1 with timer=nullptr, so future opens
			// increment to 2 and can never start deferred SDK notifications.
			try
			{
				instance.timer = VSTGUI::makeOwned<VSTGUI::CVSTGUITimer> (
				    [] (VSTGUI::CVSTGUITimer*) { gUpdateHandlerInit.get ()->triggerDeferedUpdates (); },
				    1000 / 30);
			}
			catch (...)
			{
				--instance.users;
				throw;
			}
		}
	}
]=])
  string(FIND "${_code}" "${_original}" _at)
  if(_at EQUAL -1)
    string(FIND "${_code}" "Drumator NV01: the first timer allocation may throw" _already)
    if(_already EQUAL -1)
      message(FATAL_ERROR "Drumator: pinned VSTGUI IdleUpdateHandler::start changed; patch REFUSED")
    endif()
    message(STATUS "Drumator: VSTGUI first-idle-timer allocation patch already applied")
    return()
  endif()
  string(REPLACE "${_original}" "${_replacement}" _fixed "${_code}")
  file(WRITE "${source_file}" "${_fixed}")
  message(STATUS "Drumator: patched VSTGUI IdleUpdateHandler::start exception rollback")
endfunction()
