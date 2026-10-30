# 125A Drumator — authoritative current status

**As of 30 October 2026 — V1.0.1 RELEASE CANDIDATE (not yet FINAL).**

- **Production source branch:** `develop/v1.0.1-gui-redesign`.
- **Latest product code commit:** [`25d15b66`](https://github.com/challanger2000/125A-Drumator-Final/commit/25d15b66cbf1089df773310c69afc770a5b14591).
- **F16:** fixed pre-attach VST3 `getSize` at default 130% so the host's window titlebar and actual 1664px editor width agree.
- **F17:** changed sample navigation from `< SAMPLE ... >` to `< SAMPLE > ...`, including the actual native GUI hitboxes.
- **Windows QA of latest code:** [Actions #41079094604](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41079094604) — check final run conclusion; do not mark as passed before completion.
- **Previous fully tested PRODUCT baseline:** `c90297b5ab9f44716e4d06f49dc913c643560705`, [Windows #41000904184](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41000904184): 29/29 CTests, 47/47 Steinberg Validator, 58/0/0 Plugin Tester.
- **Owner DAW feedback:** prior WAV export, LIVE editing and core sequencing were reported working. Studio One FIRST OPEN at default 130% with correctly aligned host title/close buttons **requires a fresh screenshot** on the current product build.
- **Release hold:** No FINAL/tag/Gumroad publication before exact current VST3, docs and titlebar/sampler navigation pass Studio One, and commercial redistribution rights, license/support information are confirmed.

See [release-readiness findings](docs/audits/RELEASE-READINESS-20261030.md) and [development history](docs/history/).
