BEGIN;

INSERT OR REPLACE INTO language(id,code,locale,name_native,name_english,doxygen_language,active) VALUES
(1,'de','de-DE','Deutsch','German','German',1),
(2,'en','en-US','English','English','English',1);

INSERT OR REPLACE INTO reference_category(id,code,sort_order,active) VALUES
(1,'applicability',10,1),
(2,'exposure',20,1),
(3,'effective-risk',30,1),
(4,'decision',40,1),
(5,'reason',50,1),
(6,'publication-state',60,1);

INSERT OR REPLACE INTO reference_category_text(id,category_id,language_id,label,description) VALUES
(1,1,1,'Anwendbarkeit','Bewertung, ob ein Provider-Finding auf die konkrete BuildEngine-Buildkonfiguration zutrifft.'),
(2,1,2,'Applicability','Whether a provider finding applies to the concrete BuildEngine build configuration.'),
(3,2,1,'Exposition','Art und Umfang der technischen Exposition innerhalb des erzeugten Builds.'),
(4,2,2,'Exposure','Type and extent of technical exposure within the generated build.'),
(5,3,1,'Wirksames Risiko','Nach technischer Bewertung verbleibendes Risiko fuer die konkrete Buildkonfiguration.'),
(6,3,2,'Effective risk','Risk remaining for the concrete build configuration after technical assessment.'),
(7,4,1,'Entscheidung','Dokumentierte Entscheidung fuer das Finding.'),
(8,4,2,'Decision','Documented decision for the finding.'),
(9,5,1,'Begruendung','Standardisierte technische Begruendung der Entscheidung.'),
(10,5,2,'Reason','Standardized technical reason for the decision.'),
(11,6,1,'Verteilung','Legt fest, ob eine Bewertung nur lokal oder als zentrale BuildEngine-Bewertung geteilt wird.'),
(12,6,2,'Publication','Defines whether an assessment remains local or is shared as a central BuildEngine assessment.');

INSERT OR REPLACE INTO reference_value(id,category_id,code,sort_order,active) VALUES
(101,1,'unreviewed',0,1),(102,1,'applicable',10,1),(103,1,'not-applicable',20,1),(104,1,'unknown',30,1),
(201,2,'runtime',10,1),(202,2,'build-time-only',20,1),(203,2,'test-only',30,1),(204,2,'bundled-component',40,1),
(205,2,'optional-feature',50,1),(206,2,'not-included-in-build',60,1),(207,2,'not-shipped',70,1),(208,2,'unknown',90,1),
(301,3,'none',0,1),(302,3,'low',10,1),(303,3,'medium',20,1),(304,3,'high',30,1),(305,3,'critical',40,1),(306,3,'unknown',90,1),
(401,4,'open',10,1),(402,4,'mitigated',20,1),(403,4,'accepted-risk',30,1),(404,4,'fixed-by-configuration',40,1),
(405,4,'fixed-by-update',50,1),(406,4,'closed-not-applicable',60,1),(407,4,'needs-investigation',90,1),
(501,5,'affected-feature-disabled',10,1),(502,5,'affected-component-not-built',20,1),(503,5,'affected-component-not-shipped',30,1),
(504,5,'affected-version-does-not-match',40,1),(505,5,'bundled-component-not-affected',50,1),(506,5,'exploit-precondition-not-present',60,1),
(507,5,'mitigation-configured',70,1),(508,5,'patched-locally',80,1),(509,5,'updated-version',90,1),(510,5,'provider-mapping-mismatch',100,1),(511,5,'other',999,1),
(601,6,'local',10,1),(602,6,'shared',20,1);

INSERT OR REPLACE INTO reference_value_text(id,reference_value_id,language_id,label,description) VALUES
(1001,101,1,'Nicht bewertet','Noch keine belastbare Bewertung.'),(1002,101,2,'Unreviewed','No reliable assessment yet.'),
(1003,102,1,'Anwendbar','Finding trifft auf die konkrete Buildkonfiguration zu.'),(1004,102,2,'Applicable','Finding applies to the concrete build configuration.'),
(1005,103,1,'Nicht anwendbar','Finding trifft auf die konkrete Buildkonfiguration nicht zu.'),(1006,103,2,'Not applicable','Finding does not apply to the concrete build configuration.'),
(1007,104,1,'Unklar','Bewertung ist noch nicht moeglich.'),(1008,104,2,'Unknown','Assessment is not yet possible.'),

(2001,201,1,'Laufzeit','Betroffene Funktionalitaet ist im ausgelieferten Laufzeitpaket vorhanden.'),(2002,201,2,'Runtime','Affected functionality is present in the shipped runtime package.'),
(2003,202,1,'Nur Buildzeit','Betroffene Komponente oder Funktion wird nur beim Build benoetigt.'),(2004,202,2,'Build-time only','Affected component or feature is only needed during build.'),
(2005,203,1,'Nur Tests','Betroffene Komponente oder Funktion wird nur in Tests verwendet.'),(2006,203,2,'Test-only','Affected component or feature is used only in tests.'),
(2007,204,1,'Eingebettete Komponente','Finding betrifft eine eingebettete Komponente.'),(2008,204,2,'Bundled component','Finding concerns a bundled component.'),
(2009,205,1,'Optionale Funktion','Finding betrifft eine optionale Funktion.'),(2010,205,2,'Optional feature','Finding concerns an optional feature.'),
(2011,206,1,'Nicht gebaut','Betroffene Funktion oder Komponente ist in dieser Buildkonfiguration nicht enthalten.'),(2012,206,2,'Not included in build','Affected feature or component is not included in this build configuration.'),
(2013,207,1,'Nicht ausgeliefert','Betroffene Funktion oder Komponente wird nicht ausgeliefert.'),(2014,207,2,'Not shipped','Affected feature or component is not shipped.'),
(2015,208,1,'Unklar','Exposition ist noch nicht bewertet.'),(2016,208,2,'Unknown','Exposure has not yet been assessed.'),

(3001,301,1,'Kein wirksames Risiko','Fuer die konkrete Buildkonfiguration besteht nach Bewertung kein wirksames Risiko.'),(3002,301,2,'None','No effective risk remains for the concrete build configuration after assessment.'),
(3003,302,1,'Niedrig','Niedriges wirksames Risiko.'),(3004,302,2,'Low','Low effective risk.'),
(3005,303,1,'Mittel','Mittleres wirksames Risiko.'),(3006,303,2,'Medium','Medium effective risk.'),
(3007,304,1,'Hoch','Hohes wirksames Risiko.'),(3008,304,2,'High','High effective risk.'),
(3009,305,1,'Kritisch','Kritisches wirksames Risiko.'),(3010,305,2,'Critical','Critical effective risk.'),
(3011,306,1,'Unklar','Wirksames Risiko ist noch nicht bewertet.'),(3012,306,2,'Unknown','Effective risk has not yet been assessed.'),

(4001,401,1,'Offen','Finding ist offen.'),(4002,401,2,'Open','Finding is open.'),
(4003,402,1,'Mitigiert','Risiko ist durch eine dokumentierte Massnahme reduziert.'),(4004,402,2,'Mitigated','Risk is reduced by a documented mitigation.'),
(4005,403,1,'Risiko akzeptiert','Bekanntes Risiko wurde bewusst akzeptiert.'),(4006,403,2,'Accepted risk','Known risk has been explicitly accepted.'),
(4007,404,1,'Durch Konfiguration ausgeschlossen','Die Buildkonfiguration schliesst die betroffene Funktionalitaet aus.'),(4008,404,2,'Fixed by configuration','The build configuration excludes the affected functionality.'),
(4009,405,1,'Durch Update behoben','Das Finding wurde durch ein Update behoben.'),(4010,405,2,'Fixed by update','The finding was fixed by an update.'),
(4011,406,1,'Nicht anwendbar / geschlossen','Finding ist fuer diese Buildkonfiguration nicht anwendbar.'),(4012,406,2,'Closed - not applicable','Finding is not applicable to this build configuration.'),
(4013,407,1,'Pruefung erforderlich','Weitere technische Pruefung erforderlich.'),(4014,407,2,'Needs investigation','Further technical investigation is required.'),

(5001,501,1,'Betroffene Funktion deaktiviert','Die betroffene Funktion ist durch Buildoptionen deaktiviert.'),(5002,501,2,'Affected feature disabled','The affected feature is disabled by build options.'),
(5003,502,1,'Betroffene Komponente nicht gebaut','Die betroffene Komponente wird in der konkreten Konfiguration nicht gebaut.'),(5004,502,2,'Affected component not built','The affected component is not built in the concrete configuration.'),
(5005,503,1,'Betroffene Komponente nicht ausgeliefert','Die betroffene Komponente wird nicht in das Paket uebernommen.'),(5006,503,2,'Affected component not shipped','The affected component is not included in the package.'),
(5007,504,1,'Betroffene Version trifft nicht zu','Die vom Provider genannte Versionsbedingung trifft auf die konkrete Identitaet nicht zu.'),(5008,504,2,'Affected version does not match','The provider version condition does not match the concrete identity.'),
(5009,505,1,'Eingebettete Komponente nicht betroffen','Die eingebettete Variante ist nach Pruefung nicht betroffen.'),(5010,505,2,'Bundled component not affected','The bundled variant is not affected after review.'),
(5011,506,1,'Exploit-Voraussetzung fehlt','Eine notwendige technische Voraussetzung fuer den Exploit ist nicht vorhanden.'),(5012,506,2,'Exploit precondition not present','A required technical exploit precondition is not present.'),
(5013,507,1,'Mitigation konfiguriert','Eine konkrete technische Mitigation ist aktiv.'),(5014,507,2,'Mitigation configured','A concrete technical mitigation is active.'),
(5015,508,1,'Lokal gepatcht','Die verwendete Quelle enthaelt einen dokumentierten lokalen Patch.'),(5016,508,2,'Patched locally','The used source contains a documented local patch.'),
(5017,509,1,'Aktualisierte Version','Die verwendete Version enthaelt den Fix.'),(5018,509,2,'Updated version','The used version contains the fix.'),
(5019,510,1,'Provider-Zuordnung unzutreffend','Provider-Mapping passt nicht zur tatsaechlich verwendeten Komponente oder Identitaet.'),(5020,510,2,'Provider mapping mismatch','Provider mapping does not match the actually used component or identity.'),
(5021,511,1,'Sonstiger Grund','Begruendung steht im Kommentar.'),(5022,511,2,'Other','Reason is documented in the comment.'),

(6001,601,1,'Lokal','Bewertung bleibt in der Arbeitsdatenbank und wird nicht verteilt.'),(6002,601,2,'Local','Assessment remains in the working database and is not distributed.'),
(6003,602,1,'Zentral geteilt','Bewertung gilt fuer die BuildEngine-Buildkonfiguration und darf ueber BuildEngine-Admin verteilt werden.'),(6004,602,2,'Shared centrally','Assessment applies to the BuildEngine build configuration and may be distributed through BuildEngine-Admin.');

INSERT OR REPLACE INTO meta(id,key,value) VALUES
(1,'schema-version','2'),
(2,'reference-data-version','2026-09-09'),
(3,'default-language-id','1');

COMMIT;
