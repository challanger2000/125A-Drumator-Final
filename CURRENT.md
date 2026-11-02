# 125A Drumator — authoritative current status

> **AKTUELLER ENTWICKLUNGSSTAND – 02.11.2026:** Separater Branch `develop/beat-creator-10-styles-20261102` basiert auf dem exakt geprüften 1.0.1-Quellstand. **BC41 Velocity (Paint, Line, Curve, Accent) ist vollständig aus V1.5 übernommen und CI-geprüft**, [Run #41373979469](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41373979469) SUCCESS. **BC42 Beat Creator ist programmiert**: 10 elektronische Styles / 80 Groove-Vorlagen, ein Dropdown, wählbare 16/32 Schritte, direkt bearbeitbare Complexity, A/B/C-Visualisierung und atomarer A/B/C-Apply mit bestätigtem Überschreiben und gruppiertem Undo, keine Samplekits. **BC42-VST3 ist noch nicht für Gumroad freigegeben:** Ein früherer paralleler CI-Testlauf scheiterte am 64-Stimmen-Zeitlimit (1/31 CTests); den aktuellsten [Windows-CI-Lauf #41376112322](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41376112322) kontrollieren, dann DAW-Praxistest. **Verbindliche Übergabe:** [START-HERE.md](START-HERE.md). **Original-Gumroad-ZIP vorhanden:** `125A_Drumator_v1.0.1_Gumroad_Final.zip`, byteidentisch mit `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip` (SHA256 `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`). **Nicht erneut anfordern; veröffentlichte Version 1.0.1 bleibt unverändert.**

---

**31 October 2026 | Version 1.0.1 | F21 footer alignment complete; verified updated Gumroad customer ZIP prepared.**

## Authoritative source and build

- **Main and development branch:** same complete 1.0.1 product state after release documentation synchronization. Do not use superseded F18/F19/F20 candidate ZIPs.
- **Exact product code verified:** `3748b1e9e1867357362304c51bec2cb99ac9657b`.
- **[Windows Actions #41157069202](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41157069202): SUCCESS.** 29/29 CTests, Steinberg VST3 Validator 47/47, independent 125A Plugin Tester v0.3.2 **58 PASS / 0 WARNING / 0 FAIL**.
- **Product DLL SHA256:** `06c784854ce4b96763933444c5400ced8c4f5eed5f03339ca6b0f1c071de3278`.
- **Updated complete customer ZIP:** `125A_Drumator_v1.0.1_Gumroad_Update_2026-10-31.zip`; 26 files; SHA256 `cb263d6b3a66a125b967661942f36e3011b2ce8c3995325d0e12cceccf2084eb`. Prepared outside the repository and validated for archive integrity and all per-file hashes. Both illustrated DE/EN PDFs use the actual new GUI screenshot.
- **Storefront status:** The previous 1.0.1 Gumroad package was confirmed uploaded by the owner on 30 October 2026. This new corrected ZIP **must replace that file in Gumroad**; upload by the owner has not been verified or performed here.

## Final GUI corrections within 1.0.1

- **F20 (verified in owner's Studio One screenshot):** Regular instrument pages are nonoverlapping: 1–10 then 11–16, or only row 11 for an 11-row kit. Normal 10-row viewport keeps its 686px logical height across NEXT/PREV. Unoccupied areas are inactive background. Constrained peer-window recovery remains intact.
- **F21:** Footer bezel shifted exactly 4 logical pixels left, from x16..1264 to **x12..1260**, matching the row chassis. Footer stays 1248px wide. All button rectangles, interaction hitboxes, songs, DSP and window size logic remain untouched.
- **Visual regression:** Pixel difference between prior F20 and F21 actual Windows renders is confined to the footer bezel region (130% main, 16-row second page, and 11-row second page); there are no other changed pixels.
- **Earlier 1.0.1 work retained:** PLAY/FILL/EDIT order, corrected headings and colors, aligned right-header controls, Logo aligned to PATTERN A, 66 stable automation parameters.
- **Version decision:** **1.0.1**. No 1.0.2, no new release tag.
- **QA scope:** Windows automated host/native render CI and independent plugin validation passed. The owner's previous Studio One check covers F20. A fresh manual DAW check of this F21 DLL was not performed; F21 only moves the decorative footer panel.

## Superseded items (historical only)

- F18 corrected a photo crop but did not solve DAW resizing.
- F19 used overlapping 2–11/7–16 pages and was rejected.
- Earlier `125A_Drumator_1.0.1_Next_Page_10_plus_6_Test.zip` and `125A_Drumator_v1.0.1_Gumroad_Final_GUI_Fixes.zip` are superseded for distribution.
- Older audit reports remain only as historical evidence; README and this file identify the current build.
