PRAGMA foreign_keys = ON;
PRAGMA user_version = 3;

CREATE TABLE IF NOT EXISTS meta (
   id INTEGER PRIMARY KEY,
   key TEXT NOT NULL UNIQUE,
   value TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS language (
   id INTEGER PRIMARY KEY,
   code TEXT NOT NULL UNIQUE,
   locale TEXT NOT NULL UNIQUE,
   name_native TEXT NOT NULL,
   name_english TEXT NOT NULL,
   doxygen_language TEXT NOT NULL,
   active INTEGER NOT NULL DEFAULT 1 CHECK(active IN (0,1))
);

CREATE TABLE IF NOT EXISTS reference_category (
   id INTEGER PRIMARY KEY,
   code TEXT NOT NULL UNIQUE,
   sort_order INTEGER NOT NULL DEFAULT 0,
   active INTEGER NOT NULL DEFAULT 1 CHECK(active IN (0,1))
);

CREATE TABLE IF NOT EXISTS reference_category_text (
   id INTEGER PRIMARY KEY,
   category_id INTEGER NOT NULL,
   language_id INTEGER NOT NULL,
   label TEXT NOT NULL,
   description TEXT NOT NULL DEFAULT '',
   UNIQUE(category_id, language_id),
   FOREIGN KEY(category_id) REFERENCES reference_category(id),
   FOREIGN KEY(language_id) REFERENCES language(id)
);

CREATE TABLE IF NOT EXISTS reference_value (
   id INTEGER PRIMARY KEY,
   category_id INTEGER NOT NULL,
   code TEXT NOT NULL,
   sort_order INTEGER NOT NULL DEFAULT 0,
   active INTEGER NOT NULL DEFAULT 1 CHECK(active IN (0,1)),
   UNIQUE(category_id, code),
   FOREIGN KEY(category_id) REFERENCES reference_category(id)
);

CREATE TABLE IF NOT EXISTS reference_value_text (
   id INTEGER PRIMARY KEY,
   reference_value_id INTEGER NOT NULL,
   language_id INTEGER NOT NULL,
   label TEXT NOT NULL,
   description TEXT NOT NULL DEFAULT '',
   UNIQUE(reference_value_id, language_id),
   FOREIGN KEY(reference_value_id) REFERENCES reference_value(id),
   FOREIGN KEY(language_id) REFERENCES language(id)
);

CREATE TABLE IF NOT EXISTS scan_run (
   id INTEGER PRIMARY KEY,
   started_at TEXT NOT NULL,
   completed_at TEXT,
   provider TEXT NOT NULL,
   build_file TEXT NOT NULL,
   production_root TEXT,
   status TEXT NOT NULL DEFAULT 'running',
   error TEXT
);

CREATE TABLE IF NOT EXISTS provider_finding (
   id INTEGER PRIMARY KEY,
   provider TEXT NOT NULL,
   advisory_id TEXT NOT NULL,
   primary_id TEXT NOT NULL,
   published TEXT,
   modified TEXT,
   withdrawn TEXT,
   summary TEXT,
   details TEXT,
   raw_json TEXT,
   first_seen TEXT NOT NULL,
   last_seen TEXT NOT NULL,
   UNIQUE(provider, advisory_id)
);

CREATE TABLE IF NOT EXISTS osv_observation (
   id INTEGER PRIMARY KEY,
   provider_finding_id INTEGER NOT NULL,
   observed_at TEXT NOT NULL,
   source_modified TEXT,
   fingerprint TEXT NOT NULL,
   fix_available INTEGER NOT NULL DEFAULT 0 CHECK(fix_available IN (0,1)),
   affected_versions_json TEXT NOT NULL DEFAULT '[]',
   fixed_versions_json TEXT NOT NULL DEFAULT '[]',
   last_affected_json TEXT NOT NULL DEFAULT '[]',
   ranges_json TEXT NOT NULL DEFAULT '[]',
   aliases_json TEXT NOT NULL DEFAULT '[]',
   credits_json TEXT NOT NULL DEFAULT '[]',
   database_specific_json TEXT NOT NULL DEFAULT '{}',
   severity_json TEXT NOT NULL DEFAULT '[]',
   raw_json TEXT NOT NULL DEFAULT '{}',
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id)
);

CREATE TABLE IF NOT EXISTS nvd_observation (
   id INTEGER PRIMARY KEY,
   provider_finding_id INTEGER NOT NULL,
   observed_at TEXT NOT NULL,
   source_last_modified TEXT,
   fingerprint TEXT NOT NULL,
   vuln_status TEXT,
   cvss_version TEXT,
   cvss_base_score REAL,
   cvss_base_severity TEXT,
   cvss_vector TEXT,
   cvss_exploitability_score REAL,
   cvss_impact_score REAL,
   cisa_exploit_add TEXT,
   cisa_action_due TEXT,
   cisa_required_action TEXT,
   cisa_vulnerability_name TEXT,
   weaknesses_json TEXT NOT NULL DEFAULT '[]',
   configurations_json TEXT NOT NULL DEFAULT '[]',
   references_json TEXT NOT NULL DEFAULT '[]',
   raw_json TEXT NOT NULL DEFAULT '{}',
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id)
);

CREATE TABLE IF NOT EXISTS epss_observation (
   id INTEGER PRIMARY KEY,
   provider_finding_id INTEGER NOT NULL,
   observed_at TEXT NOT NULL,
   score_date TEXT,
   fingerprint TEXT NOT NULL,
   epss_score REAL,
   percentile REAL,
   raw_json TEXT NOT NULL DEFAULT '{}',
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id)
);

CREATE TABLE IF NOT EXISTS kev_observation (
   id INTEGER PRIMARY KEY,
   provider_finding_id INTEGER NOT NULL,
   observed_at TEXT NOT NULL,
   catalog_version TEXT,
   catalog_date_released TEXT,
   fingerprint TEXT NOT NULL,
   is_known_exploited INTEGER NOT NULL CHECK(is_known_exploited IN (0,1)),
   date_added TEXT,
   due_date TEXT,
   known_ransomware_campaign_use TEXT,
   vendor_project TEXT,
   product TEXT,
   vulnerability_name TEXT,
   required_action TEXT,
   notes TEXT,
   raw_json TEXT NOT NULL DEFAULT '{}',
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id)
);

CREATE TABLE IF NOT EXISTS build_finding (
   id INTEGER PRIMARY KEY,
   library_id TEXT NOT NULL,
   library_version TEXT NOT NULL,
   component_name TEXT NOT NULL,
   component_version TEXT,
   component_purl TEXT,
   component_bom_ref TEXT,
   identity_repository TEXT,
   identity_ref TEXT,
   identity_commit TEXT,
   build_timestamp TEXT,
   sbom_sha256 TEXT,
   build_contract_fingerprint TEXT,
   provider_finding_id INTEGER NOT NULL,
   provider_severity TEXT,
   first_seen TEXT NOT NULL,
   last_seen TEXT NOT NULL,
   is_current INTEGER NOT NULL DEFAULT 1 CHECK(is_current IN (0,1)),
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id)
);

CREATE UNIQUE INDEX IF NOT EXISTS ux_build_finding_identity
   ON build_finding(library_id, library_version, component_name, IFNULL(component_version, ''), IFNULL(identity_commit, ''), provider_finding_id);

CREATE TABLE IF NOT EXISTS assessment (
   id INTEGER PRIMARY KEY,
   finding_id INTEGER NOT NULL,
   revision INTEGER NOT NULL,
   applicability_id INTEGER NOT NULL,
   exposure_id INTEGER NOT NULL,
   effective_risk_id INTEGER NOT NULL,
   decision_id INTEGER NOT NULL,
   reason_id INTEGER NOT NULL,
   publication_state_id INTEGER NOT NULL,
   comment TEXT NOT NULL DEFAULT '',
   reviewer TEXT NOT NULL,
   created_at TEXT NOT NULL,
   supersedes INTEGER,
   evidence_fingerprint TEXT NOT NULL,
   FOREIGN KEY(finding_id) REFERENCES build_finding(id),
   FOREIGN KEY(applicability_id) REFERENCES reference_value(id),
   FOREIGN KEY(exposure_id) REFERENCES reference_value(id),
   FOREIGN KEY(effective_risk_id) REFERENCES reference_value(id),
   FOREIGN KEY(decision_id) REFERENCES reference_value(id),
   FOREIGN KEY(reason_id) REFERENCES reference_value(id),
   FOREIGN KEY(publication_state_id) REFERENCES reference_value(id),
   FOREIGN KEY(supersedes) REFERENCES assessment(id),
   UNIQUE(finding_id, revision)
);

CREATE INDEX IF NOT EXISTS ix_build_finding_library ON build_finding(library_id, library_version, is_current);
CREATE INDEX IF NOT EXISTS ix_assessment_finding ON assessment(finding_id, revision DESC);
CREATE INDEX IF NOT EXISTS ix_reference_value_category ON reference_value(category_id, sort_order, id);
CREATE INDEX IF NOT EXISTS ix_osv_observation_finding ON osv_observation(provider_finding_id, id DESC);
CREATE INDEX IF NOT EXISTS ix_nvd_observation_finding ON nvd_observation(provider_finding_id, id DESC);
CREATE INDEX IF NOT EXISTS ix_epss_observation_finding ON epss_observation(provider_finding_id, id DESC);
CREATE INDEX IF NOT EXISTS ix_kev_observation_finding ON kev_observation(provider_finding_id, id DESC);

CREATE VIEW IF NOT EXISTS current_assessment AS
SELECT a.* FROM assessment a
JOIN (SELECT finding_id, MAX(revision) revision FROM assessment GROUP BY finding_id) latest
ON latest.finding_id=a.finding_id AND latest.revision=a.revision;

CREATE VIEW IF NOT EXISTS latest_osv_observation AS
SELECT o.* FROM osv_observation o
JOIN (SELECT provider_finding_id, MAX(id) id FROM osv_observation GROUP BY provider_finding_id) latest ON latest.id=o.id;
CREATE VIEW IF NOT EXISTS latest_nvd_observation AS
SELECT o.* FROM nvd_observation o
JOIN (SELECT provider_finding_id, MAX(id) id FROM nvd_observation GROUP BY provider_finding_id) latest ON latest.id=o.id;
CREATE VIEW IF NOT EXISTS latest_epss_observation AS
SELECT o.* FROM epss_observation o
JOIN (SELECT provider_finding_id, MAX(id) id FROM epss_observation GROUP BY provider_finding_id) latest ON latest.id=o.id;
CREATE VIEW IF NOT EXISTS latest_kev_observation AS
SELECT o.* FROM kev_observation o
JOIN (SELECT provider_finding_id, MAX(id) id FROM kev_observation GROUP BY provider_finding_id) latest ON latest.id=o.id;

CREATE VIEW IF NOT EXISTS current_external_vulnerability AS
SELECT pf.id provider_finding_id,pf.primary_id,
 oo.observed_at osv_observed_at,oo.source_modified osv_source_modified,oo.fix_available,oo.affected_versions_json,oo.fixed_versions_json,oo.last_affected_json,oo.ranges_json,
 no.observed_at nvd_observed_at,no.source_last_modified nvd_last_modified,no.vuln_status nvd_status,no.cvss_version,no.cvss_base_score,no.cvss_base_severity,no.cvss_vector,no.cvss_exploitability_score,no.cvss_impact_score,
 eo.observed_at epss_observed_at,eo.score_date epss_date,eo.epss_score,eo.percentile epss_percentile,
 ko.observed_at kev_observed_at,ko.is_known_exploited,ko.date_added kev_date_added,ko.due_date kev_due_date,ko.known_ransomware_campaign_use,ko.required_action kev_required_action
FROM provider_finding pf
LEFT JOIN latest_osv_observation oo ON oo.provider_finding_id=pf.id
LEFT JOIN latest_nvd_observation no ON no.provider_finding_id=pf.id
LEFT JOIN latest_epss_observation eo ON eo.provider_finding_id=pf.id
LEFT JOIN latest_kev_observation ko ON ko.provider_finding_id=pf.id;

CREATE VIEW IF NOT EXISTS review_queue AS
SELECT f.id finding_id,f.library_id,f.library_version,f.component_name,f.component_version,f.identity_commit,pf.provider,pf.advisory_id,f.provider_severity,pf.primary_id,pf.summary,
 a.id assessment_id,a.revision assessment_revision,a.applicability_id,a.exposure_id,a.effective_risk_id,a.decision_id,a.reason_id,a.publication_state_id,a.evidence_fingerprint assessment_evidence_fingerprint,f.build_contract_fingerprint,f.sbom_sha256
FROM build_finding f JOIN provider_finding pf ON pf.id=f.provider_finding_id
LEFT JOIN current_assessment a ON a.finding_id=f.id WHERE f.is_current=1;

CREATE VIEW IF NOT EXISTS shared_current_assessment AS
SELECT a.* FROM current_assessment a
JOIN reference_value publication_state ON publication_state.id=a.publication_state_id
JOIN reference_category publication_category ON publication_category.id=publication_state.category_id
WHERE publication_category.code='publication-state' AND publication_state.code='shared';
