BEGIN;

INSERT OR REPLACE INTO reference_value(category,code,label_de,label_en,description_de,sort_order,active) VALUES
('applicability','unreviewed','Nicht bewertet','Unreviewed','Noch keine belastbare Bewertung.',0,1),
('applicability','applicable','Anwendbar','Applicable','Finding trifft auf die konkrete Buildkonfiguration zu.',10,1),
('applicability','not-applicable','Nicht anwendbar','Not applicable','Finding trifft auf die konkrete Buildkonfiguration nicht zu.',20,1),
('applicability','unknown','Unklar','Unknown','Bewertung ist noch nicht moeglich.',30,1),

('exposure','runtime','Laufzeit','Runtime','Betroffene Funktionalitaet ist im ausgelieferten Laufzeitpaket vorhanden.',10,1),
('exposure','build-time-only','Nur Buildzeit','Build-time only','Betroffene Komponente oder Funktion wird nur beim Build benoetigt.',20,1),
('exposure','test-only','Nur Tests','Test-only','Betroffene Komponente oder Funktion wird nur in Tests verwendet.',30,1),
('exposure','bundled-component','Eingebettete Komponente','Bundled component','Finding betrifft eine eingebettete Komponente.',40,1),
('exposure','optional-feature','Optionale Funktion','Optional feature','Finding betrifft eine optionale Funktion.',50,1),
('exposure','not-included-in-build','Nicht gebaut','Not included in build','Betroffene Funktion oder Komponente ist in dieser Buildkonfiguration nicht enthalten.',60,1),
('exposure','not-shipped','Nicht ausgeliefert','Not shipped','Betroffene Funktion oder Komponente wird nicht ausgeliefert.',70,1),
('exposure','unknown','Unklar','Unknown','Exposition ist noch nicht bewertet.',90,1),

('effective-risk','none','Kein wirksames Risiko','None','Fuer die konkrete Buildkonfiguration besteht nach Bewertung kein wirksames Risiko.',0,1),
('effective-risk','low','Niedrig','Low','Niedriges wirksames Risiko.',10,1),
('effective-risk','medium','Mittel','Medium','Mittleres wirksames Risiko.',20,1),
('effective-risk','high','Hoch','High','Hohes wirksames Risiko.',30,1),
('effective-risk','critical','Kritisch','Critical','Kritisches wirksames Risiko.',40,1),
('effective-risk','unknown','Unklar','Unknown','Wirksames Risiko ist noch nicht bewertet.',90,1),

('decision','open','Offen','Open','Finding ist offen.',10,1),
('decision','mitigated','Mitigiert','Mitigated','Risiko ist durch eine dokumentierte Massnahme reduziert.',20,1),
('decision','accepted-risk','Risiko akzeptiert','Accepted risk','Bekanntes Risiko wurde bewusst akzeptiert.',30,1),
('decision','fixed-by-configuration','Durch Konfiguration ausgeschlossen','Fixed by configuration','Die Buildkonfiguration schliesst die betroffene Funktionalitaet aus.',40,1),
('decision','fixed-by-update','Durch Update behoben','Fixed by update','Das Finding wurde durch ein Update behoben.',50,1),
('decision','closed-not-applicable','Nicht anwendbar / geschlossen','Closed - not applicable','Finding ist fuer diese Buildkonfiguration nicht anwendbar.',60,1),
('decision','needs-investigation','Pruefung erforderlich','Needs investigation','Weitere technische Pruefung erforderlich.',90,1),

('reason','affected-feature-disabled','Betroffene Funktion deaktiviert','Affected feature disabled','Die betroffene Funktion ist durch Buildoptionen deaktiviert.',10,1),
('reason','affected-component-not-built','Betroffene Komponente nicht gebaut','Affected component not built','Die betroffene Komponente wird in der konkreten Konfiguration nicht gebaut.',20,1),
('reason','affected-component-not-shipped','Betroffene Komponente nicht ausgeliefert','Affected component not shipped','Die betroffene Komponente wird nicht in das Paket uebernommen.',30,1),
('reason','affected-version-does-not-match','Betroffene Version trifft nicht zu','Affected version does not match','Die vom Provider genannte Versionsbedingung trifft auf die konkrete Identitaet nicht zu.',40,1),
('reason','bundled-component-not-affected','Eingebettete Komponente nicht betroffen','Bundled component not affected','Die eingebettete Variante ist nach Pruefung nicht betroffen.',50,1),
('reason','exploit-precondition-not-present','Exploit-Voraussetzung fehlt','Exploit precondition not present','Eine notwendige technische Voraussetzung fuer den Exploit ist nicht vorhanden.',60,1),
('reason','mitigation-configured','Mitigation konfiguriert','Mitigation configured','Eine konkrete technische Mitigation ist aktiv.',70,1),
('reason','patched-locally','Lokal gepatcht','Patched locally','Die verwendete Quelle enthaelt einen dokumentierten lokalen Patch.',80,1),
('reason','updated-version','Aktualisierte Version','Updated version','Die verwendete Version enthaelt den Fix.',90,1),
('reason','provider-mapping-mismatch','Provider-Zuordnung unzutreffend','Provider mapping mismatch','Provider-Mapping passt nicht zur tatsaechlich verwendeten Komponente oder Identitaet.',100,1),
('reason','other','Sonstiger Grund','Other','Begruendung steht im Kommentar.',999,1);

COMMIT;
