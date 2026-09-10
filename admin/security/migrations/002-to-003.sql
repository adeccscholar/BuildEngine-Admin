PRAGMA foreign_keys = ON;
BEGIN IMMEDIATE;

CREATE TABLE IF NOT EXISTS osv_observation (
   id INTEGER PRIMARY KEY,
   provider_finding_id INTEGER NOT NULL,
   observed_at TEXT NOT NULL,
   source_modified TEXT,
   fingerprint TEXT NOT NULL,
   fix_available INTEGER NOT NULL DEFAULT 0 CHECK(fix_available IN (0,1)),
   affected_versions_json TEXT NOT NULL DEFAULT '[]',
   ranges_json TEXT NOT NULL DEFAULT '[]',
   aliases_json TEXT NOT NULL DEFAULT '[]',
   credits_json TEXT NOT NULL DEFAULT '[]',
   database_specific_json TEXT NOT NULL DEFAULT '{}',
   severity_json TEXT NOT NULL DEFAULT '[]',
   raw_json TEXT NOT NULL DEFAULT '{}',
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id),
   UNIQUE(provider_finding_id, fingerprint)
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
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id),
   UNIQUE(provider_finding_id, fingerprint)
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
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id),
   UNIQUE(provider_finding_id, fingerprint)
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
   FOREIGN KEY(provider_finding_id) REFERENCES provider_finding(id),
   UNIQUE(provider_finding_id, fingerprint)
);

CREATE INDEX IF NOT EXISTS ix_osv_observation_finding ON osv_observation(provider_finding_id, id DESC);
CREATE INDEX IF NOT EXISTS ix_nvd_observation_finding ON nvd_observation(provider_finding_id, id DESC);
CREATE INDEX IF NOT EXISTS ix_epss_observation_finding ON epss_observation(provider_finding_id, id DESC);
CREATE INDEX IF NOT EXISTS ix_kev_observation_finding ON kev_observation(provider_finding_id, id DESC);

DROP VIEW IF EXISTS current_external_vulnerability;
DROP VIEW IF EXISTS latest_osv_observation;
DROP VIEW IF EXISTS latest_nvd_observation;
DROP VIEW IF EXISTS latest_epss_observation;
DROP VIEW IF EXISTS latest_kev_observation;

CREATE VIEW latest_osv_observation AS
SELECT o.* FROM osv_observation o
JOIN (SELECT provider_finding_id, MAX(id) id FROM osv_observation GROUP BY provider_finding_id) latest ON latest.id=o.id;
CREATE VIEW latest_nvd_observation AS
SELECT o.* FROM nvd_observation o
JOIN (SELECT provider_finding_id, MAX(id) id FROM nvd_observation GROUP BY provider_finding_id) latest ON latest.id=o.id;
CREATE VIEW latest_epss_observation AS
SELECT o.* FROM epss_observation o
JOIN (SELECT provider_finding_id, MAX(id) id FROM epss_observation GROUP BY provider_finding_id) latest ON latest.id=o.id;
CREATE VIEW latest_kev_observation AS
SELECT o.* FROM kev_observation o
JOIN (SELECT provider_finding_id, MAX(id) id FROM kev_observation GROUP BY provider_finding_id) latest ON latest.id=o.id;

CREATE VIEW current_external_vulnerability AS
SELECT
   pf.id AS provider_finding_id,
   pf.primary_id,
   oo.observed_at AS osv_observed_at,
   oo.source_modified AS osv_source_modified,
   oo.fix_available,
   oo.affected_versions_json,
   oo.ranges_json,
   no.observed_at AS nvd_observed_at,
   no.source_last_modified AS nvd_last_modified,
   no.vuln_status AS nvd_status,
   no.cvss_version,
   no.cvss_base_score,
   no.cvss_base_severity,
   no.cvss_vector,
   no.cvss_exploitability_score,
   no.cvss_impact_score,
   eo.observed_at AS epss_observed_at,
   eo.score_date AS epss_date,
   eo.epss_score,
   eo.percentile AS epss_percentile,
   ko.observed_at AS kev_observed_at,
   ko.is_known_exploited,
   ko.date_added AS kev_date_added,
   ko.due_date AS kev_due_date,
   ko.known_ransomware_campaign_use,
   ko.required_action AS kev_required_action
FROM provider_finding pf
LEFT JOIN latest_osv_observation oo ON oo.provider_finding_id=pf.id
LEFT JOIN latest_nvd_observation no ON no.provider_finding_id=pf.id
LEFT JOIN latest_epss_observation eo ON eo.provider_finding_id=pf.id
LEFT JOIN latest_kev_observation ko ON ko.provider_finding_id=pf.id;

PRAGMA user_version = 3;
COMMIT;
