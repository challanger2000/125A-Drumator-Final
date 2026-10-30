> **HISTORISCHES ORIGINAL AUS DEM VERALTETEN MAIN 1.0.0.** Diese Datei wurde zur Archivierung am 30.10.2026 abgelegt. Bitte keine Freigabe-/Versionsaussagen daraus als aktuell verwenden. Maßgeblich: [aktuelle 1.0.1-Statusdatei](https://github.com/challanger2000/125A-Drumator-Final/blob/develop/v1.0.1-gui-redesign/CURRENT.md).

---

# 125A Drumator — Current

- Target Windows VST3 version: **1.0.0**
- Status: **V1.0.0 Windows QA passed; owner field test in Studio One in progress; no public release yet**
- Source repo: `challanger2000/125A-Drumator`
- Audited product commit: `a14d9e67470ad0af94a8250baca13fa3a15fb70e`
- Source export (docs-only additional commit): `91afadfd643fd32fa49e3d0fdb4ec9e39c9e106b`
- Windows original source QA: [run 40425541707](https://github.com/challanger2000/125A-Drumator/actions/runs/40425541707), 23/23 CTests, 47/47 Validator, 58/0/0 Tester
- Final repository Windows QA: **PASS** at [Actions #40430043280](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40430043280), commit `69d2e24ff0311516d20e274da95ad69f50e75692`: **23/23 CTests; 47/47 Steinberg Validator; packaged VST3 audio/host PASS; 125A Plugin Tester v0.3.2: 58 PASS / 0 WARNING / 0 FAIL**; exact candidate DLL SHA-256 `3366e1a0bdff3553cad4cabec3b24780b0414ddf0e16299e4d7c86e616dcbcb4`
- Owner-local release-candidate ZIP: `125A_Drumator_v1.0.0_Windows_x64_FINAL.zip` (manually renamed by owner; public package upload/Gumroad release **NOT DONE**)
- Final packaging fix (unused background excluded, moduleinfo 1.0.0): [Windows QA #40432613982](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/40432613982), source commit `e84df4ea8c317e12bf792a0a2396c810ed2d6f02`, **SUCCESS**
- Publication: **ON HOLD** pending several days of Studio One use and owner decision; do not change audio, GUI, resource files, or publish without explicit approval
- Git submodules: **none** in the copied source tree; third-party VST3 SDK is fetched at build time
