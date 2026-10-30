# Dokumentenwegweiser — 125A Drumator 1.0.1

**Aktuell (30.10.2026): Windows x64 VST3, technischer Windows-Prüflauf erfolgreich, kommerzielles Kundenpaket vorbereitet, nicht öffentlich veröffentlicht.**

## Aktuell und maßgeblich
- [CURRENT.md](../CURRENT.md): **einzige maßgebliche Statusdatei**, inklusive geprüftem Produkt-SHA, CI, ZIP und Veröffentlichungsstatus
- [README.md](../README.md): aktuelle Funktionen, Installation und Branch-Navigation
- [AGENTS.md](../AGENTS.md): Regeln für Code-, Build- und QA-Änderungen
- [BEDIENUNGSANLEITUNG-DE.md](BEDIENUNGSANLEITUNG-DE.md): aktualisierte deutsche Quelldokumentation
- [SDK-BASELINE.md](SDK-BASELINE.md): gepinnte Versionen
- [Windows-QA #41125370883, Versuch 2](https://github.com/challanger2000/125A-Drumator-Final/actions/runs/41125370883): 29/29 CTests, 47/47 Validator, 58/0/0 Plugin Tester

**Produkt-Commit:** `1acfe3698bcbe6eb88e1faee6ff8e538edbc390d`. Alle späteren commits bis zur Statusaktualisierung betreffen ausschließlich Dokumentation.

## GitHub-Branches
- **`develop/v1.0.1-gui-redesign`**: aktueller Code und gültige technische Dokumentation
- **`main`**: alter 1.0.0-Quellstand; GitHub-Standardbranch, **nicht identisch mit 1.0.1**. Nicht unbemerkt für Builds oder Freigaben verwenden. Eine spätere Merge-Entscheidung ist getrennt zu treffen.

## Historische Dokumentation
Die Root-Auditdateien `TECHNISCHER-AUDIT.md`, `CODEX-FINAL-AUDIT.md`, `AUDIT-PRE-VINTAGE-GUI-2026-10-17.md` und `CODEX-DND-ANALYSE.md`, die Inhalte unter [docs/audits](audits), [docs/history](history), die Meilensteine sowie datierte Technik-/GUI-Berichte dokumentieren ihre **jeweiligen damaligen SHAs**. Früheres „FINAL“, veraltete Funktionsgrenzen und offene Defekte dürfen nicht auf den neuen Build übertragen werden.

Der [Release-Readiness-Audit](audits/RELEASE-READINESS-20261030.md) ist deshalb mit einem deutlichen Historienhinweis versehen.

## Vertrieb
Das überprüfte externe Kundenarchiv lautet `125A_Drumator_1.0.1_Gumroad_Kundenpaket_mit_Lizenzhinweisen.zip`. ZIP-Hash, VST3-Binärhash und Produktversion stehen in [CURRENT.md](../CURRENT.md). Dieses ZIP liegt **nicht** im Git-Repository. Kein GitHub Release und kein Release-Tag; keine bestätigte Gumroad-Veröffentlichung. Technische QA ersetzt keine gesonderte Praxisprüfung in der DAW.
