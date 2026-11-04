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
  # A comment alone is NEVER evidence of an applied security/lifetime patch.
  # Accept exactly ONE pristine block or exactly ONE complete known replacement.
  # Even a partially edited replacement (e.g. missing --instance.users) fails.
  string(FIND "${_code}" "${_original}" _original_first)
  string(FIND "${_code}" "${_replacement}" _replacement_first)
  if(NOT _original_first EQUAL -1 AND NOT _replacement_first EQUAL -1)
    message(FATAL_ERROR "Drumator: both pristine and patched IdleUpdateHandler::start found")
  endif()
  if(NOT _original_first EQUAL -1)
    string(LENGTH "${_original}" _orig_len)
    math(EXPR _orig_next "${_original_first}+${_orig_len}")
    string(SUBSTRING "${_code}" ${_orig_next} -1 _orig_tail)
    string(FIND "${_orig_tail}" "${_original}" _orig_duplicate)
    if(NOT _orig_duplicate EQUAL -1)
      message(FATAL_ERROR "Drumator: duplicated original IdleUpdateHandler::start; patch REFUSED")
    endif()
    string(REPLACE "${_original}" "${_replacement}" _fixed "${_code}")
    file(WRITE "${source_file}" "${_fixed}")
    message(STATUS "Drumator: patched VSTGUI IdleUpdateHandler::start exception rollback")
    return()
  endif()
  if(NOT _replacement_first EQUAL -1)
    string(LENGTH "${_replacement}" _replacement_len)
    math(EXPR _replacement_next "${_replacement_first}+${_replacement_len}")
    string(SUBSTRING "${_code}" ${_replacement_next} -1 _replacement_tail)
    string(FIND "${_replacement_tail}" "${_replacement}" _replacement_duplicate)
    if(NOT _replacement_duplicate EQUAL -1)
      message(FATAL_ERROR "Drumator: duplicated patched IdleUpdateHandler::start; patch REFUSED")
    endif()
    message(STATUS "Drumator: verified complete existing NV01 patch (idempotent)")
    return()
  endif()
  message(FATAL_ERROR "Drumator: unknown/mutated IdleUpdateHandler::start (including partial rollback); patch REFUSED")
endfunction()
