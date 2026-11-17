-- BuildEngine PostgreSQL core and security history.
-- Connect to database "BuildEngine" before applying this migration.
-- Requires PostgreSQL >= 18 and 001-cve-cache.sql.
-- No existing tables are removed or rewritten.
BEGIN;

CREATE SCHEMA IF NOT EXISTS buildengine_core;

CREATE TABLE IF NOT EXISTS buildengine_core.library (
   library_id text PRIMARY KEY,
   display_name text NOT NULL,
   category text NOT NULL DEFAULT '',
   description text NOT NULL DEFAULT '',
   first_seen_at timestamptz NOT NULL DEFAULT statement_timestamp(),
   updated_at timestamptz NOT NULL DEFAULT statement_timestamp()
);

CREATE TABLE IF NOT EXISTS buildengine_core.library_version (
   library_id text NOT NULL REFERENCES buildengine_core.library(library_id),
   version text NOT NULL,
   xml_timestamp timestamptz,
   superseded_by text,
   license_signal text CHECK (license_signal IN ('green', 'yellow', 'red')),
   first_seen_at timestamptz NOT NULL DEFAULT statement_timestamp(),
   last_seen_at timestamptz NOT NULL DEFAULT statement_timestamp(),
   CONSTRAINT pk_library_version PRIMARY KEY (library_id, version)
);

-- Immutable XML revisions. The full XML and SHA-256 are retained so that
-- changes to configuration are independently auditable.
CREATE TABLE IF NOT EXISTS buildengine_core.xml_revision (
   revision_id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
   source_path text NOT NULL,
   content_sha256 char(64) NOT NULL,
   observed_at timestamptz NOT NULL DEFAULT statement_timestamp(),
   xml_content text NOT NULL,
   parser_version text,
   CONSTRAINT uk_xml_revision_source_sha UNIQUE (source_path, content_sha256)
);

CREATE TABLE IF NOT EXISTS buildengine_core.library_version_revision (
   revision_id bigint NOT NULL REFERENCES buildengine_core.xml_revision(revision_id),
   library_id text NOT NULL,
   version text NOT NULL,
   raw_definition jsonb NOT NULL DEFAULT '{}'::jsonb
      CHECK (jsonb_typeof(raw_definition) = 'object'),
   CONSTRAINT pk_library_version_revision PRIMARY KEY (revision_id, library_id, version),
   CONSTRAINT fk_library_version_revision_version FOREIGN KEY (library_id, version)
      REFERENCES buildengine_core.library_version(library_id, version)
);

CREATE TABLE IF NOT EXISTS buildengine_core.dependency_revision (
   revision_id bigint NOT NULL REFERENCES buildengine_core.xml_revision(revision_id),
   source_library_id text NOT NULL,
   source_version text NOT NULL,
   target_library_id text NOT NULL,
   target_version text NOT NULL,
   relation text NOT NULL CHECK (relation IN ('declared', 'direct', 'transitive', 'bundled', 'extension')),
   CONSTRAINT pk_dependency_revision PRIMARY KEY (
      revision_id, source_library_id, source_version,
      target_library_id, target_version, relation
   )
);

-- Stable advisory identities are distinct from version-specific findings.
CREATE TABLE IF NOT EXISTS buildengine_security.advisory (
   advisory_id text PRIMARY KEY,
   primary_cve_id text,
   provider text NOT NULL,
   first_seen_at timestamptz NOT NULL DEFAULT statement_timestamp()
);

CREATE TABLE IF NOT EXISTS buildengine_security.advisory_alias (
   advisory_id text NOT NULL REFERENCES buildengine_security.advisory(advisory_id),
   alias_id text NOT NULL,
   CONSTRAINT pk_advisory_alias PRIMARY KEY (advisory_id, alias_id)
);

-- Every observed metric is tied to a scan generation and observation time.
-- Never UPDATE historical metric rows in place.
CREATE TABLE IF NOT EXISTS buildengine_security.metric_observation (
   observation_id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
   snapshot_id bigint NOT NULL REFERENCES buildengine_security.cve_snapshot(snapshot_id),
   library_id text NOT NULL,
   library_version text NOT NULL,
   advisory_id text NOT NULL REFERENCES buildengine_security.advisory(advisory_id),
   observed_at timestamptz NOT NULL DEFAULT statement_timestamp(),
   provider_updated_at timestamptz,
   cvss_version text,
   cvss_vector text,
   cvss_score numeric(3,1) CHECK (cvss_score BETWEEN 0 AND 10),
   epss_score double precision CHECK (epss_score BETWEEN 0 AND 1),
   epss_percentile double precision CHECK (epss_percentile BETWEEN 0 AND 1),
   kev_status text NOT NULL DEFAULT 'unknown'
      CHECK (kev_status IN ('yes', 'no', 'unknown')),
   fix_available boolean,
   evidence jsonb NOT NULL DEFAULT '{}'::jsonb
      CHECK (jsonb_typeof(evidence) = 'object'),
   CONSTRAINT uk_metric_observation_coordinate UNIQUE (
      snapshot_id, library_id, library_version, advisory_id
   )
);

CREATE INDEX IF NOT EXISTS ix_metric_observation_history
   ON buildengine_security.metric_observation
   (library_id, library_version, advisory_id, observed_at DESC);

-- Controlled assessment value tables. No user-supplied free-text codes
-- are used as state identifiers.
CREATE TABLE IF NOT EXISTS buildengine_security.dom_assessment_status (
   status_code text PRIMARY KEY,
   description text NOT NULL
);
INSERT INTO buildengine_security.dom_assessment_status (status_code, description)
VALUES
   ('open', 'Assessment is awaiting review'),
   ('in_review', 'A reviewer is evaluating applicability'),
   ('approved', 'Assessment has been approved'),
   ('rejected', 'Assessment was rejected'),
   ('superseded', 'Assessment was superseded by a newer decision')
ON CONFLICT (status_code) DO NOTHING;

CREATE TABLE IF NOT EXISTS buildengine_security.dom_applicability (
   applicability_code text PRIMARY KEY,
   description text NOT NULL
);
INSERT INTO buildengine_security.dom_applicability (applicability_code, description)
VALUES
   ('unknown', 'Applicability has not been established'),
   ('affected', 'The installed build is affected'),
   ('not_affected', 'The installed build is not affected based on reviewed evidence'),
   ('conditional', 'Affected only when documented conditions hold')
ON CONFLICT (applicability_code) DO NOTHING;

CREATE TABLE IF NOT EXISTS buildengine_security.dom_assessment_disposition (
   disposition_code text PRIMARY KEY,
   description text NOT NULL
);
INSERT INTO buildengine_security.dom_assessment_disposition (disposition_code, description)
VALUES
   ('investigate', 'Further evidence is needed'),
   ('mitigate', 'Mitigation is planned or in progress'),
   ('upgrade', 'Upgrade or rebuild is required'),
   ('accept', 'Residual risk was explicitly accepted'),
   ('not_applicable', 'The finding does not apply to this build')
ON CONFLICT (disposition_code) DO NOTHING;

CREATE TABLE IF NOT EXISTS buildengine_security.assessment (
   assessment_id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
   library_id text NOT NULL,
   library_version text NOT NULL,
   advisory_id text NOT NULL REFERENCES buildengine_security.advisory(advisory_id),
   created_at timestamptz NOT NULL DEFAULT statement_timestamp(),
   created_by text NOT NULL,
   CONSTRAINT fk_assessment_version FOREIGN KEY (library_id, library_version)
      REFERENCES buildengine_core.library_version(library_id, version)
);

-- Append-only decisions preserve who assessed which exact finding and why.
CREATE TABLE IF NOT EXISTS buildengine_security.assessment_revision (
   assessment_revision_id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
   assessment_id bigint NOT NULL REFERENCES buildengine_security.assessment(assessment_id),
   revised_at timestamptz NOT NULL DEFAULT statement_timestamp(),
   reviewed_by text NOT NULL,
   status_code text NOT NULL REFERENCES buildengine_security.dom_assessment_status(status_code),
   applicability_code text NOT NULL REFERENCES buildengine_security.dom_applicability(applicability_code),
   disposition_code text NOT NULL REFERENCES buildengine_security.dom_assessment_disposition(disposition_code),
   justification text NOT NULL,
   evidence jsonb NOT NULL DEFAULT '{}'::jsonb
      CHECK (jsonb_typeof(evidence) = 'object'),
   supersedes_revision_id bigint REFERENCES buildengine_security.assessment_revision(assessment_revision_id),
   CONSTRAINT ck_assessment_revision_justification CHECK (length(btrim(justification)) > 0)
);
CREATE INDEX IF NOT EXISTS ix_assessment_revision_history
   ON buildengine_security.assessment_revision (assessment_id, revised_at DESC);

CREATE OR REPLACE VIEW buildengine_security.latest_assessment AS
SELECT DISTINCT ON (a.assessment_id)
   a.assessment_id, a.library_id, a.library_version, a.advisory_id,
   r.assessment_revision_id, r.revised_at, r.reviewed_by,
   r.status_code, r.applicability_code, r.disposition_code, r.justification, r.evidence
FROM buildengine_security.assessment AS a
JOIN buildengine_security.assessment_revision AS r
   ON r.assessment_id = a.assessment_id
ORDER BY a.assessment_id, r.revised_at DESC, r.assessment_revision_id DESC;

COMMIT;
