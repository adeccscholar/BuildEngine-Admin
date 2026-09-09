PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS meta (
   key TEXT PRIMARY KEY,
   value TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS reference_value (
   category TEXT NOT NULL,
   code TEXT NOT NULL,
   label_de TEXT NOT NULL,
   label_en TEXT NOT NULL,
   description_de TEXT NOT NULL DEFAULT '',
   sort_order INTEGER NOT NULL DEFAULT 0,
   active INTEGER NOT NULL DEFAULT 1 CHECK(active IN (0,1)),
   PRIMARY KEY(category, code)
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
   PRIMARY KEY(provider, advisory_id)
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
   provider TEXT NOT NULL,
   advisory_id TEXT NOT NULL,
   provider_severity TEXT,
   first_seen TEXT NOT NULL,
   last_seen TEXT NOT NULL,
   is_current INTEGER NOT NULL DEFAULT 1 CHECK(is_current IN (0,1)),
   FOREIGN KEY(provider, advisory_id)
      REFERENCES provider_finding(provider, advisory_id)
);

CREATE UNIQUE INDEX IF NOT EXISTS ux_build_finding_identity
   ON build_finding(
      library_id,
      library_version,
      component_name,
      IFNULL(component_version, ''),
      IFNULL(identity_commit, ''),
      provider,
      advisory_id
   );

CREATE TABLE IF NOT EXISTS assessment (
   id INTEGER PRIMARY KEY,
   finding_id INTEGER NOT NULL,
   revision INTEGER NOT NULL,
   applicability TEXT NOT NULL,
   exposure TEXT NOT NULL,
   effective_risk TEXT NOT NULL,
   decision TEXT NOT NULL,
   reason_code TEXT NOT NULL,
   comment TEXT NOT NULL DEFAULT '',
   reviewer TEXT NOT NULL,
   created_at TEXT NOT NULL,
   supersedes INTEGER,
   evidence_fingerprint TEXT NOT NULL,
   publication_state TEXT NOT NULL DEFAULT 'local'
      CHECK(publication_state IN ('local','shared')),
   FOREIGN KEY(finding_id) REFERENCES build_finding(id),
   FOREIGN KEY(supersedes) REFERENCES assessment(id),
   UNIQUE(finding_id, revision)
);

CREATE INDEX IF NOT EXISTS ix_build_finding_library
   ON build_finding(library_id, library_version, is_current);

CREATE INDEX IF NOT EXISTS ix_assessment_finding
   ON assessment(finding_id, revision DESC);

CREATE VIEW IF NOT EXISTS current_assessment AS
SELECT a.*
FROM assessment AS a
JOIN (
   SELECT finding_id, MAX(revision) AS revision
   FROM assessment
   GROUP BY finding_id
) AS latest
ON latest.finding_id = a.finding_id
AND latest.revision = a.revision;

CREATE VIEW IF NOT EXISTS review_queue AS
SELECT
   f.id AS finding_id,
   f.library_id,
   f.library_version,
   f.component_name,
   f.component_version,
   f.identity_commit,
   f.provider,
   f.advisory_id,
   f.provider_severity,
   p.primary_id,
   p.summary,
   COALESCE(a.applicability, 'unreviewed') AS applicability,
   COALESCE(a.exposure, 'unknown') AS exposure,
   COALESCE(a.effective_risk, 'unknown') AS effective_risk,
   COALESCE(a.decision, 'needs-investigation') AS decision,
   COALESCE(a.publication_state, 'local') AS publication_state
FROM build_finding AS f
JOIN provider_finding AS p
  ON p.provider = f.provider AND p.advisory_id = f.advisory_id
LEFT JOIN current_assessment AS a
  ON a.finding_id = f.id
WHERE f.is_current = 1;
