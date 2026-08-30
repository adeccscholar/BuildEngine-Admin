# TODO – Boost Versions-Upgrade und Discovery

## Reihenfolge

Dieser Punkt wird **erst nach Abschluss des aktuellen Boost-1.92.0-Nachweises und nach Integration der weiteren vorgesehenen Third-Party-Libraries** umgesetzt.

Aktuelle Priorität:

1. Boost 1.92.0 vollständig über Build, Install, Publish und alle Release-/Debug-Evidence-Gates abschließen.
2. Weitere Bibliotheken in die BuildEngine aufnehmen und jeweils über denselben reproduzierbaren Paket-/Consumer-Vertrag verifizieren.
3. Danach Boost-Version-Discovery und Upgrade-Preflight automatisieren.

## Ziel

Ein Wechsel auf eine neue Boost-Version soll nicht durch blindes Übernehmen des 1.92.0-Profils erfolgen. Die BuildEngine soll die neue Upstream-Version inventarisieren und die Abweichungen zum zuletzt akzeptierten Boost-Profil sichtbar machen.

## Geplanter Upgrade-Preflight

Für eine neue Boost-Version:

- offizielles Upstream-Artefakt und SHA-256 neu pinnen;
- logische Boost-Libraries und CMake-Source-Module aus der Upstream-Metadatenstruktur neu inventarisieren;
- Diff gegen das zuletzt akzeptierte Profil erzeugen: hinzugefügt, entfernt, umbenannt, Metadaten-/Targetänderungen;
- externe Ökosystem-Abhängigkeiten wie OpenCL, MPI und Python neu bewerten;
- binäre Komponentenfamilien und CMake-Targettypen neu bestimmen bzw. verifizieren;
- insbesondere Änderungen zwischen SHARED-/STATIC-/INTERFACE-Targets sichtbar machen;
- Boost.Config/BCC64X native-Clang-Preflight ohne historische Annahmen erneut ausführen;
- prüfen, ob der lokale Boost.Config-Adapter weiterhin erforderlich ist oder Upstream BCC64X inzwischen selbst korrekt klassifiziert;
- erst nach erfolgreichem Preflight den neuen kanonischen Komponentenvertrag erzeugen/akzeptieren;
- anschließend den vollständigen Release-/Debug-Produktionsgraphen sowie alle Component-/Runtime-Gates ausführen.

## Leitprinzip

Die generische Boost-Engine bleibt versionsunabhängig. Versionsgebunden sind ausschließlich das akzeptierte Komponentenprofil, die erwarteten Artefakte/Targettypen und dokumentierte externe Ausschlüsse.

Eine neue Version darf deshalb nicht allein deshalb als vollständig unterstützt gelten, weil der alte 1.92.0-Komponentensatz weiterhin baut.
