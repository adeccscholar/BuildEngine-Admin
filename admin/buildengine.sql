-- BuildEngine PostgreSQL schema: single greenfield baseline.
-- Target: a new, EMPTY PostgreSQL database named BuildEngine.
-- Source: admin/build-libraries.xml, with independent CVE enrichment.
-- This file replaces the earlier 001/002 migration drafts and verify-baseline.sql.
-- Provision with a PostgreSQL administrator (normally postgres) that may
-- CREATE ROLE and SET ROLE. The database "BuildEngine" must already exist.
-- Both application roles have LOGIN without a stored password; SSPI or other
-- authentication is configured in PostgreSQL outside this SQL file.
BEGIN;

DO $provision$
BEGIN
   IF current_database() <> 'BuildEngine' THEN
      RAISE EXCEPTION 'Connect to database BuildEngine before provisioning';
   END IF;

   IF NOT EXISTS (SELECT 1 FROM pg_roles WHERE rolname = 'buildengine_user') THEN
      CREATE ROLE buildengine_user LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE NOREPLICATION;
   END IF;

   IF NOT EXISTS (SELECT 1 FROM pg_roles WHERE rolname = 'buildengine_reader') THEN
      CREATE ROLE buildengine_reader LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE NOREPLICATION;
   END IF;

   -- A read-only service role must not inherit the owner's write privileges.
   IF pg_has_role('buildengine_reader', 'buildengine_user', 'MEMBER') THEN
      RAISE EXCEPTION 'buildengine_reader must not be a member of buildengine_user';
   END IF;
END;
$provision$;

-- Enforce the intended login role capabilities if roles already exist.
ALTER ROLE buildengine_user LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE NOREPLICATION;
ALTER ROLE buildengine_reader LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE NOREPLICATION;

-- In the dedicated BuildEngine database no anonymous/PUBLIC connection or
-- temporary-table privilege is required for the service account.
REVOKE CONNECT, TEMPORARY ON DATABASE "BuildEngine" FROM PUBLIC;
GRANT CONNECT, TEMPORARY, CREATE ON DATABASE "BuildEngine" TO buildengine_user;
GRANT CONNECT ON DATABASE "BuildEngine" TO buildengine_reader;

-- The owner role must own the schema AND all schema objects, not merely hold
-- INSERT/UPDATE/DELETE grants; that is what permits CREATE/ALTER/DROP.
CREATE SCHEMA buildengine AUTHORIZATION buildengine_user;
SET LOCAL ROLE buildengine_user;

-- Reference domains contain descriptions and notices; new categories can be
-- added through regular INSERT during catalog reconciliation.
CREATE TABLE buildengine.dom_category (
   category_id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
   category_code text NOT NULL UNIQUE,
   name text NOT NULL,
   description text NOT NULL DEFAULT '',
   notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_category(category_code,name) VALUES
   ('archive','Archive'),('compression','Compression'),
   ('concurrency','Concurrency'),('core','Core'),('data','Data'),
   ('database','Database'),('documentation','Documentation'),
   ('foundation','Foundation'),('graphics','Graphics'),('image','Image'),
   ('math','Mathematics'),('middleware','Middleware'),
   ('network','Network'),('security','Security'),
   ('testing','Testing'),('text','Text'),('uncategorized','Uncategorized');

CREATE TABLE buildengine.dom_license_signal (
   license_signal_id integer PRIMARY KEY,
   signal_code text NOT NULL UNIQUE,
   name text NOT NULL,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_license_signal VALUES
   (10,'green','Green','Normal attribution or notice obligations.',
      'Not a legal opinion. Check the upstream license.'),
   (20,'yellow','Yellow','Requires product-specific legal or integration review.',
      'Do not imply automatic permission to redistribute.'),
   (30,'red','Red','Not suitable as an in-process proprietary dependency without a separate decision.',
      'Review licensing and alternative integration.');

/**/
CREATE TABLE buildengine.dom_artifact_type (
   artifact_type_id integer PRIMARY KEY,
   type_code text NOT NULL UNIQUE,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_artifact_type VALUES
   (10,'shared','Shared binary and import-library package',''),
   (20,'static','Static library package',''),
   (30,'interface','Headers or consumer-side implementation',''),
   (40,'tool','Standalone build or runtime tool','');

CREATE TABLE buildengine.dom_relation_type (
   relation_type_id integer PRIMARY KEY,
   relation_code text NOT NULL UNIQUE,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_relation_type VALUES
   (10,'dependency','Declared dependency on another library version',''),
   (20,'extended','Extension of an existing library version',
       'Technical shared paths remain current build configuration, not a separate history.'),
   (30,'supersededBy','Old version is replaced by another version of the same library',
       'Source and target are versions of the same library.'),
   (40,'direct','Verified direct package or runtime dependency',''),
   (50,'transitive','Resolved indirect dependency',''),
   (60,'bundled','Contained third-party library or source dependency',
       'Do not confuse the bundled component with an independent installed package.');

CREATE TABLE buildengine.dom_catalog_sync_status (
   status_id integer PRIMARY KEY,
   status_code text NOT NULL UNIQUE,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_catalog_sync_status VALUES
   (10,'running','XML synchronization is in progress',''),
   (20,'complete','All XML source records were processed','Unchanged data counts as processed.'),
   (30,'partial','Some source records accepted; others rejected or unprocessed','Diagnose the missing records.'),
   (40,'failed','The XML import failed entirely; no source records were accepted','');

-- Logical FSM states known to the BuildEngine; these are stage identities,
-- not write permissions or runtime statuses. "ready" is derived and has no
-- independent persisted .state evidence.
CREATE TABLE buildengine.dom_library_state (
   state_id integer PRIMARY KEY,
   state_code text NOT NULL UNIQUE,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_library_state(state_id,state_code,description,notice) VALUES
   (10,'source','Source preparation completed',''),
   (20,'build','Compilation/link scope completed','Scope can distinguish Debug and Release.'),
   (30,'test','Library tests completed',''),
   (40,'validation','Validation scope completed',''),
   (50,'install','Installation scope completed',''),
   (60,'metadata','License and SBOM generation completed',''),
   (70,'publish','Publication scope completed',''),
   (80,'documentation','Documentation scope completed','May contain dynamically materialized scopes.'),
   (90,'ready','Logical library ready state','Derived from the FSM; no .state file is written for ready.');

-- Coverage is separate from CVE findings: zero findings after a successful
-- scan is not equivalent to unscanned or failed provider work.
CREATE TABLE buildengine.dom_security_query_status (
   status_id integer PRIMARY KEY,
   status_code text NOT NULL UNIQUE,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_security_query_status VALUES
   (10,'not-scanned','Version has not yet been checked',''),
   (20,'scanned','Version has a complete recorded security check','A zero-CVE result is meaningful only here.'),
   (30,'incomplete','Not every required source could be evaluated',''),
   (40,'failed','Version security evaluation failed','');

CREATE TABLE buildengine.dom_cve_source (
   source_id integer PRIMARY KEY,
   source_code text NOT NULL UNIQUE,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_cve_source VALUES
   (10,'OSV','Open Source Vulnerabilities',''),
   (20,'CVE.org','Official CVE records',''),
   (30,'NVD','National Vulnerability Database',''),
   (40,'EPSS','FIRST Exploit Prediction Scoring System',''),
   (50,'CISA KEV','CISA Known Exploited Vulnerabilities','');

-- Six established assessment domains. Separate tables permit simple, strongly
-- typed foreign keys without a cross-dimension trigger or generic ORM.
CREATE TABLE buildengine.dom_applicability (
   value_id integer PRIMARY KEY, code text NOT NULL UNIQUE,
   description text NOT NULL, notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_applicability(value_id,code,description) VALUES
   (101,'unreviewed','Not reviewed'),(102,'applicable','Finding applies to this build'),
   (103,'not-applicable','Finding does not apply to this build'),
   (104,'unknown','Applicability could not be established');

CREATE TABLE buildengine.dom_exposure (
   value_id integer PRIMARY KEY, code text NOT NULL UNIQUE,
   description text NOT NULL, notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_exposure(value_id,code,description) VALUES
   (201,'runtime','Present in shipped runtime'),(202,'build-time-only','Used only at build time'),
   (203,'test-only','Used only for tests'),(204,'bundled-component','Embedded source component'),
   (205,'optional-feature','Affected optional functionality'),
   (206,'not-included-in-build','Affected functionality is not built'),
   (207,'not-shipped','Affected functionality is not shipped'),
   (208,'unknown','Exposure not established');

CREATE TABLE buildengine.dom_effective_risk (
   value_id integer PRIMARY KEY, code text NOT NULL UNIQUE,
   description text NOT NULL, notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_effective_risk(value_id,code,description) VALUES
   (301,'none','No effective risk'),(302,'low','Low effective risk'),
   (303,'medium','Medium effective risk'),(304,'high','High effective risk'),
   (305,'critical','Critical effective risk'),(306,'unknown','Risk not established');

CREATE TABLE buildengine.dom_decision (
   value_id integer PRIMARY KEY, code text NOT NULL UNIQUE,
   description text NOT NULL, notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_decision(value_id,code,description) VALUES
   (401,'open','Still open'),(402,'mitigated','Mitigated'),
   (403,'accepted-risk','Risk explicitly accepted'),
   (404,'fixed-by-configuration','Configuration excludes vulnerability'),
   (405,'fixed-by-update','Update addresses vulnerability'),
   (406,'closed-not-applicable','Not applicable'),
   (407,'needs-investigation','Technical investigation required');

CREATE TABLE buildengine.dom_reason (
   value_id integer PRIMARY KEY, code text NOT NULL UNIQUE,
   description text NOT NULL, notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_reason(value_id,code,description) VALUES
   (501,'affected-feature-disabled','Feature disabled'),
   (502,'affected-component-not-built','Affected component not built'),
   (503,'affected-component-not-shipped','Affected component not shipped'),
   (504,'affected-version-does-not-match','Version mismatch'),
   (505,'bundled-component-not-affected','Bundled component unaffected'),
   (506,'exploit-precondition-not-present','Exploit precondition absent'),
   (507,'mitigation-configured','Mitigation configured'),
   (508,'patched-locally','Locally patched'),
   (509,'updated-version','Replaced by updated version'),
   (510,'provider-mapping-mismatch','Provider package mapping mismatch'),
   (511,'other','Other documented reason');

CREATE TABLE buildengine.dom_publication_state (
   value_id integer PRIMARY KEY, code text NOT NULL UNIQUE,
   description text NOT NULL, notice text NOT NULL DEFAULT ''
);
INSERT INTO buildengine.dom_publication_state(value_id,code,description) VALUES
   (601,'local','Local assessment'),(602,'shared','Shared assessment');

-- Library metadata is stored ONCE per library, not repeated from XML versions.
CREATE TABLE buildengine.library (
   library_id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
   library_code text NOT NULL UNIQUE,
   name text NOT NULL,
   description text NOT NULL DEFAULT '',
   supplier text,
   homepage text,
   category_id bigint NOT NULL REFERENCES buildengine.dom_category(category_id),
   license_signal_id integer NOT NULL REFERENCES buildengine.dom_license_signal(license_signal_id),
   license_notice text,
   created_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   modified_at timestamptz NOT NULL DEFAULT clock_timestamp()
);

-- version_no is a stable positive ordinal allocated within each library.
-- version_text is NOT parsed or ordered to determine its identity.
CREATE TABLE buildengine.library_version (
   library_id bigint NOT NULL REFERENCES buildengine.library(library_id),
   version_no bigint NOT NULL,
   version_text text NOT NULL,
   -- Source presence is reconciled by the latest completed XML import.
   -- Keep removed versions for CVE and historical relation references.
   in_catalog boolean NOT NULL DEFAULT true,
   source_file text,
   description text,
   license_notice text,
   license_signal_id integer REFERENCES buildengine.dom_license_signal(license_signal_id),
   license_evidence text,
   artifact_type_id integer REFERENCES buildengine.dom_artifact_type(artifact_type_id),
   runtime_dll text,
   -- Exact text of sbom/bundled-components.json (or the configured components.json).
   -- Not a per-component relational structure and not independently versioned.
   components_manifest text,
   sbom_sha256 char(64),
   sbom_path text,
   -- Server cache of actual installed files: not authoritative build evidence.
   sbom_document text,
   license_info_document text,
   artifact_manifest text,
   package_present boolean NOT NULL DEFAULT false,
   documentation_present boolean NOT NULL DEFAULT false,
   cache_checked_at timestamptz,
   security_repository_url text,
   security_repository_ref text,
   security_repository_commit text,
   original_contract_timestamp timestamptz NOT NULL,
   current_contract_timestamp timestamptz NOT NULL,
   created_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   modified_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   PRIMARY KEY(library_id,version_no),
   CONSTRAINT uk_library_version_text UNIQUE(library_id,version_text),
   CONSTRAINT ck_version_positive CHECK(version_no>0)
);
CREATE INDEX ix_library_version_text ON buildengine.library_version(version_text);

-- Database MIRROR of expected logical scopes, not their source of truth.
-- BuildEngine reads the .state file and the required artifact evidence first.
-- One row per currently configured state scope; completed_at is NULL when
-- missing, invalid, stale, or not yet completed. On successful FS Commit(),
-- copy the EXACT timestamp returned by ScopeStateStore::Commit().
-- A source XML change can invalidate old state evidence without deleting it.
-- Always reconcile all expected scopes after XML load and when a scope changes.
CREATE TABLE buildengine.library_scope_state (
   library_id bigint NOT NULL,
   version_no bigint NOT NULL,
   state_id integer NOT NULL
      REFERENCES buildengine.dom_library_state(state_id),
   scope_name text NOT NULL,
   source_contract_timestamp text NOT NULL,
   evidence_file text NOT NULL,
   completed_at timestamptz,
   checked_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   CONSTRAINT pk_library_scope_state PRIMARY KEY(
      library_id,version_no,state_id,scope_name),
   CONSTRAINT fk_library_scope_version FOREIGN KEY(library_id,version_no)
      REFERENCES buildengine.library_version(library_id,version_no),
   CONSTRAINT ck_library_scope_name CHECK(length(btrim(scope_name))>0),
   CONSTRAINT ck_library_scope_timestamp CHECK(length(btrim(source_contract_timestamp))>0),
   CONSTRAINT ck_library_scope_file CHECK(length(btrim(evidence_file))>0)
);
CREATE INDEX ix_library_scope_state_current ON
   buildengine.library_scope_state(library_id,version_no,state_id,completed_at);

-- One dated row is one historical relation state. No relation_id, no
-- revision table, no extension-shared-path table. Target version and
-- relation_type_id are CONTENT, not the logical relationship key.
-- A newer effective_from replaces the version/status displayed for
-- (source library, source version, target library).
CREATE TABLE buildengine.library_relation (
   source_library_id bigint NOT NULL,
   source_version_no bigint NOT NULL,
   target_library_id bigint NOT NULL,
   target_version_no bigint NOT NULL,
   effective_from timestamptz NOT NULL,
   recorded_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   relation_type_id integer NOT NULL REFERENCES buildengine.dom_relation_type(relation_type_id),
   is_present boolean NOT NULL DEFAULT true,
   notice text,
   CONSTRAINT pk_library_relation PRIMARY KEY(
      source_library_id,source_version_no,target_library_id,effective_from),
   CONSTRAINT fk_relation_source FOREIGN KEY(source_library_id,source_version_no)
      REFERENCES buildengine.library_version(library_id,version_no),
   CONSTRAINT fk_relation_target FOREIGN KEY(target_library_id,target_version_no)
      REFERENCES buildengine.library_version(library_id,version_no),
   CONSTRAINT ck_relation_distinct CHECK(
      source_library_id<>target_library_id OR source_version_no<>target_version_no)
);
CREATE INDEX ix_library_relation_target ON buildengine.library_relation(
   target_library_id,target_version_no);
CREATE INDEX ix_library_relation_date ON buildengine.library_relation(
   source_library_id,source_version_no,target_library_id,effective_from DESC);

-- Import is a LOG of whole-file attempts, not a catalog snapshot/generation.
-- SHA-256 is intentionally nonunique so an unchanged XML can be retried.
CREATE TABLE buildengine.catalog_sync (
   sync_id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
   source_file text NOT NULL,
   source_sha256 char(64) NOT NULL,
   source_schema_version integer,
   started_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   completed_at timestamptz,
   status_id integer NOT NULL DEFAULT 10
      REFERENCES buildengine.dom_catalog_sync_status(status_id),
   libraries_count integer NOT NULL DEFAULT 0,
   versions_count integer NOT NULL DEFAULT 0,
   relations_count integer NOT NULL DEFAULT 0,
   inserted_count integer NOT NULL DEFAULT 0,
   updated_count integer NOT NULL DEFAULT 0,
   unchanged_count integer NOT NULL DEFAULT 0,
   rejected_count integer NOT NULL DEFAULT 0,
   unprocessed_count integer NOT NULL DEFAULT 0,
   diagnostic_summary text NOT NULL DEFAULT '',
   CONSTRAINT ck_catalog_sync_counts CHECK(
      inserted_count>=0 AND updated_count>=0 AND unchanged_count>=0
      AND rejected_count>=0 AND unprocessed_count>=0
      AND libraries_count>=0 AND versions_count>=0 AND relations_count>=0)
);
CREATE INDEX ix_catalog_sync_hash ON buildengine.catalog_sync(
   source_file,source_sha256);
CREATE INDEX ix_catalog_sync_completion ON buildengine.catalog_sync(
   completed_at DESC,sync_id DESC);

-- Current validated tool registry mirror (admin/build-tools.xml,
-- admin/build-source-tools.xml, admin/tools.xml); no historical tool snapshots.
CREATE TABLE buildengine.tool_cache (
   tool_code text PRIMARY KEY,
   tool_version text,
   tool_kind text,
   registered_path text,
   is_available boolean NOT NULL DEFAULT false,
   checked_at timestamptz NOT NULL DEFAULT clock_timestamp()
);

-- Current standalone test/showcase metadata discovered below tests/.
-- Keep the complete showcase.json source as text, avoiding artificial
-- child tables for its applications and library references.
CREATE TABLE buildengine.showcase_cache (
   showcase_path text PRIMARY KEY,
   showcase_manifest text,
   is_available boolean NOT NULL DEFAULT false,
   checked_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   CONSTRAINT ck_showcase_cache_path CHECK(length(btrim(showcase_path))>0)
);

-- CVEs are independent of catalog_sync, XML import dates and BuildEngine runs.
-- Current narrative is updated in place when new evidence extends the content.
CREATE TABLE buildengine.cve (
   cve_id bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
   cve_code text NOT NULL UNIQUE,
   title text NOT NULL DEFAULT '',
   description text NOT NULL DEFAULT '',
   references_text text,
   advisory_identifiers text,
   source_details text,
   first_seen_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   modified_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   last_checked_at timestamptz,
   CONSTRAINT ck_cve_code CHECK(cve_code ~ '^CVE-[0-9]{4}-[0-9]{4,}$')
);

CREATE TABLE buildengine.version_cve (
   library_id bigint NOT NULL,
   version_no bigint NOT NULL,
   cve_id bigint NOT NULL REFERENCES buildengine.cve(cve_id),
   first_seen_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   last_seen_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   evidence text,
   CONSTRAINT pk_version_cve PRIMARY KEY(library_id,version_no,cve_id),
   CONSTRAINT fk_version_cve_library FOREIGN KEY(library_id,version_no)
      REFERENCES buildengine.library_version(library_id,version_no)
);
CREATE INDEX ix_version_cve_id ON buildengine.version_cve(cve_id);

-- Independent per-version security coverage, even if zero CVEs were found.
-- This current status is updated on each check; provider ratings and reviewed
-- assessments alone have change-triggered chronological history.
CREATE TABLE buildengine.version_security_status (
   library_id bigint NOT NULL,
   version_no bigint NOT NULL,
   status_id integer NOT NULL DEFAULT 10
      REFERENCES buildengine.dom_security_query_status(status_id),
   checked_at timestamptz,
   diagnostic_summary text NOT NULL DEFAULT '',
   PRIMARY KEY(library_id,version_no),
   CONSTRAINT fk_security_status_version FOREIGN KEY(library_id,version_no)
      REFERENCES buildengine.library_version(library_id,version_no)
);

-- Append rows ONLY when a provider's evaluating values have changed.
-- last_checked_at on cve is updated without creating a rating revision.
CREATE TABLE buildengine.cve_rating (
   cve_id bigint NOT NULL REFERENCES buildengine.cve(cve_id),
   source_id integer NOT NULL REFERENCES buildengine.dom_cve_source(source_id),
   changed_at timestamptz NOT NULL,
   recorded_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   cvss_score numeric(3,1),
   cvss_version text,
   cvss_vector text,
   exploitability_score numeric(4,1),
   impact_score numeric(4,1),
   epss_score double precision,
   epss_percentile double precision,
   kev_listed boolean,
   note text,
   CONSTRAINT pk_cve_rating PRIMARY KEY(cve_id,source_id,changed_at),
   CONSTRAINT ck_cve_score CHECK(cvss_score BETWEEN 0 AND 10),
   CONSTRAINT ck_cve_exploitability CHECK(exploitability_score BETWEEN 0 AND 10),
   CONSTRAINT ck_cve_impact CHECK(impact_score BETWEEN 0 AND 10),
   CONSTRAINT ck_cve_epss CHECK(epss_score BETWEEN 0 AND 1),
   CONSTRAINT ck_cve_epss_percentile CHECK(epss_percentile BETWEEN 0 AND 1)
);

-- Each assessment is a complete six-dimension judgment for this exact CVE
-- and library version. The dated key is its history; no assessment ID or
-- revision counter. Add a row only if the assessment materially changes.
CREATE TABLE buildengine.cve_assessment (
   library_id bigint NOT NULL,
   version_no bigint NOT NULL,
   cve_id bigint NOT NULL,
   changed_at timestamptz NOT NULL,
   recorded_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   reviewer text NOT NULL,
   applicability_id integer NOT NULL REFERENCES buildengine.dom_applicability(value_id),
   exposure_id integer NOT NULL REFERENCES buildengine.dom_exposure(value_id),
   effective_risk_id integer NOT NULL REFERENCES buildengine.dom_effective_risk(value_id),
   decision_id integer NOT NULL REFERENCES buildengine.dom_decision(value_id),
   reason_id integer NOT NULL REFERENCES buildengine.dom_reason(value_id),
   publication_state_id integer NOT NULL REFERENCES buildengine.dom_publication_state(value_id),
   comment text NOT NULL DEFAULT '',
   evidence text,
   CONSTRAINT pk_cve_assessment PRIMARY KEY(library_id,version_no,cve_id,changed_at),
   CONSTRAINT fk_cve_assessment_version_cve FOREIGN KEY(library_id,version_no,cve_id)
      REFERENCES buildengine.version_cve(library_id,version_no,cve_id)
);

-- Server views: simple current state plus history.
CREATE VIEW buildengine.v_library_versions AS
SELECT l.library_id,l.library_code,l.name,l.description AS library_description,
       l.supplier,l.homepage,
       cat.category_code,cat.name AS category_name,cat.description AS category_description,
       l.license_notice AS library_license_notice,
       sig.signal_code AS license_signal,
       v.version_no,v.version_text,v.in_catalog,v.description AS version_description,
       v.license_notice AS version_license_notice,
       v.license_evidence,v.components_manifest,
       atype.type_code AS artifact_type,v.runtime_dll,
       v.sbom_sha256,v.sbom_path,
       v.security_repository_url,v.security_repository_ref,v.security_repository_commit,
       v.original_contract_timestamp,v.current_contract_timestamp,
       v.created_at AS version_created_at,v.modified_at AS version_modified_at
FROM buildengine.library AS l
JOIN buildengine.library_version AS v USING(library_id)
JOIN buildengine.dom_category AS cat ON cat.category_id=l.category_id
JOIN buildengine.dom_license_signal AS sig
   ON sig.license_signal_id=COALESCE(v.license_signal_id,l.license_signal_id)
LEFT JOIN buildengine.dom_artifact_type AS atype
   ON atype.artifact_type_id=v.artifact_type_id;

CREATE VIEW buildengine.v_library_runtime_cache AS
SELECT v.library_id,v.version_no,v.version_text,
       v.package_present,v.documentation_present,
       v.sbom_sha256,v.sbom_path,
       v.sbom_document IS NOT NULL AS sbom_available,
       v.license_info_document IS NOT NULL AS license_info_available,
       v.artifact_manifest IS NOT NULL AS artifact_manifest_available,
       v.cache_checked_at,
       ss.status_code AS security_status,
       sec.checked_at AS security_checked_at,sec.diagnostic_summary AS security_diagnostic
FROM buildengine.library_version AS v
LEFT JOIN buildengine.version_security_status AS sec
   USING(library_id,version_no)
LEFT JOIN buildengine.dom_security_query_status AS ss
   ON ss.status_id=sec.status_id;

CREATE VIEW buildengine.v_library_relation_history AS
SELECT r.*,
       lead(r.effective_from) OVER (
          PARTITION BY r.source_library_id,r.source_version_no,r.target_library_id
          ORDER BY r.effective_from
       ) AS effective_until
FROM buildengine.library_relation AS r;

-- First identify the latest state of each relation, THEN filter removals.
CREATE VIEW buildengine.v_library_relation_current AS
SELECT h.*
FROM buildengine.v_library_relation_history AS h
WHERE h.effective_from<=CURRENT_TIMESTAMP
   AND (h.effective_until IS NULL OR CURRENT_TIMESTAMP<h.effective_until)
   AND h.is_present;

-- No FSM decisions may read these views; they are for the server cache.
-- Every configured scope must be written (including not-completed scopes) so
-- bool_and can only report a state as complete when all scopes are present.
-- The server should also check checked_at for cache freshness.
CREATE VIEW buildengine.v_library_state_cache AS
SELECT sc.library_id,sc.version_no,sc.state_id,d.state_code,
       COUNT(*) AS expected_scopes,
       COUNT(sc.completed_at) AS completed_scopes,
       bool_and(sc.completed_at IS NOT NULL) AS is_complete,
       CASE WHEN bool_and(sc.completed_at IS NOT NULL)
            THEN MAX(sc.completed_at) ELSE NULL END AS completed_at,
       MAX(sc.checked_at) AS last_checked_at
FROM buildengine.library_scope_state AS sc
JOIN buildengine.dom_library_state AS d USING(state_id)
GROUP BY sc.library_id,sc.version_no,sc.state_id,d.state_code;

CREATE VIEW buildengine.v_catalog_sync_history AS
SELECT s.*,d.status_code,d.description AS status_description
FROM buildengine.catalog_sync AS s
JOIN buildengine.dom_catalog_sync_status AS d USING(status_id);

CREATE VIEW buildengine.v_catalog_sync_latest AS
SELECT * FROM buildengine.v_catalog_sync_history
ORDER BY started_at DESC,sync_id DESC LIMIT 1;

CREATE VIEW buildengine.v_catalog_sync_last_complete AS
SELECT * FROM buildengine.v_catalog_sync_history
WHERE status_id=20 AND completed_at IS NOT NULL
ORDER BY completed_at DESC,sync_id DESC LIMIT 1;

CREATE VIEW buildengine.v_cve_max_severity AS
SELECT c.cve_id,c.cve_code,MAX(r.cvss_score) AS max_cvss,
       MAX(r.epss_score) AS max_epss
FROM buildengine.cve AS c
LEFT JOIN buildengine.cve_rating AS r USING(cve_id)
GROUP BY c.cve_id,c.cve_code;

CREATE VIEW buildengine.v_cve_rating_current AS
SELECT DISTINCT ON (cve_id,source_id) *
FROM buildengine.cve_rating
ORDER BY cve_id,source_id,changed_at DESC;

CREATE VIEW buildengine.v_cve_assessment_current AS
SELECT DISTINCT ON (library_id,version_no,cve_id) *
FROM buildengine.cve_assessment
ORDER BY library_id,version_no,cve_id,changed_at DESC;

-- The service login may only read existing tables/views and run SELECT-based
-- reports. It has no schema CREATE, no table write, no sequence usage and no
-- database TEMPORARY privilege.
REVOKE ALL ON SCHEMA buildengine FROM PUBLIC;
GRANT USAGE ON SCHEMA buildengine TO buildengine_reader;
GRANT SELECT ON ALL TABLES IN SCHEMA buildengine TO buildengine_reader;

-- This rule is owned by buildengine_user (current role via SET LOCAL ROLE)
-- and applies to new tables and views created by that role in this schema.
ALTER DEFAULT PRIVILEGES IN SCHEMA buildengine
   GRANT SELECT ON TABLES TO buildengine_reader;

-- PostgreSQL otherwise permits PUBLIC EXECUTE on future functions. Keep new
-- application functions private by default. Approve any read-only reporting
-- function individually before granting it to the service role.
ALTER DEFAULT PRIVILEGES REVOKE EXECUTE ON FUNCTIONS FROM PUBLIC;

-- EXISTING-SCHEMA-UPGRADE-BEGIN

-- Idempotent migration from the original 18 November 2026 catalog baseline.

-- Preserve all library/version identities, original timestamps, and CVE history.

ALTER TABLE buildengine.library_version ADD COLUMN IF NOT EXISTS in_catalog boolean NOT NULL DEFAULT true;

ALTER TABLE buildengine.library_version ADD COLUMN IF NOT EXISTS source_file text;

ALTER TABLE buildengine.library_version ADD COLUMN IF NOT EXISTS sbom_document text;

ALTER TABLE buildengine.library_version ADD COLUMN IF NOT EXISTS license_info_document text;

ALTER TABLE buildengine.library_version ADD COLUMN IF NOT EXISTS artifact_manifest text;

ALTER TABLE buildengine.library_version ADD COLUMN IF NOT EXISTS package_present boolean NOT NULL DEFAULT false;

ALTER TABLE buildengine.library_version ADD COLUMN IF NOT EXISTS documentation_present boolean NOT NULL DEFAULT false;

ALTER TABLE buildengine.library_version ADD COLUMN IF NOT EXISTS cache_checked_at timestamptz;

CREATE TABLE IF NOT EXISTS buildengine.dom_library_state (
   state_id integer PRIMARY KEY,
   state_code text NOT NULL UNIQUE,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS buildengine.dom_security_query_status (
   status_id integer PRIMARY KEY,
   status_code text NOT NULL UNIQUE,
   description text NOT NULL,
   notice text NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS buildengine.library_scope_state (
   library_id bigint NOT NULL,
   version_no bigint NOT NULL,
   state_id integer NOT NULL
      REFERENCES buildengine.dom_library_state(state_id),
   scope_name text NOT NULL,
   source_contract_timestamp text NOT NULL,
   evidence_file text NOT NULL,
   completed_at timestamptz,
   checked_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   CONSTRAINT pk_library_scope_state PRIMARY KEY(
      library_id,version_no,state_id,scope_name),
   CONSTRAINT fk_library_scope_version FOREIGN KEY(library_id,version_no)
      REFERENCES buildengine.library_version(library_id,version_no),
   CONSTRAINT ck_library_scope_name CHECK(length(btrim(scope_name))>0),
   CONSTRAINT ck_library_scope_timestamp CHECK(length(btrim(source_contract_timestamp))>0),
   CONSTRAINT ck_library_scope_file CHECK(length(btrim(evidence_file))>0)
);

CREATE TABLE IF NOT EXISTS buildengine.tool_cache (
   tool_code text PRIMARY KEY,
   tool_version text,
   tool_kind text,
   registered_path text,
   is_available boolean NOT NULL DEFAULT false,
   checked_at timestamptz NOT NULL DEFAULT clock_timestamp()
);

CREATE TABLE IF NOT EXISTS buildengine.showcase_cache (
   showcase_path text PRIMARY KEY,
   showcase_manifest text,
   is_available boolean NOT NULL DEFAULT false,
   checked_at timestamptz NOT NULL DEFAULT clock_timestamp(),
   CONSTRAINT ck_showcase_cache_path CHECK(length(btrim(showcase_path))>0)
);

CREATE TABLE IF NOT EXISTS buildengine.version_security_status (
   library_id bigint NOT NULL,
   version_no bigint NOT NULL,
   status_id integer NOT NULL DEFAULT 10
      REFERENCES buildengine.dom_security_query_status(status_id),
   checked_at timestamptz,
   diagnostic_summary text NOT NULL DEFAULT '',
   PRIMARY KEY(library_id,version_no),
   CONSTRAINT fk_security_status_version FOREIGN KEY(library_id,version_no)
      REFERENCES buildengine.library_version(library_id,version_no)
);

INSERT INTO buildengine.dom_library_state(state_id,state_code,description,notice) VALUES
   (10,'source','Source preparation completed',''),
   (20,'build','Compilation/link scope completed','Scope can distinguish Debug and Release.'),
   (30,'test','Library tests completed',''),
   (40,'validation','Validation scope completed',''),
   (50,'install','Installation scope completed',''),
   (60,'metadata','License and SBOM generation completed',''),
   (70,'publish','Publication scope completed',''),
   (80,'documentation','Documentation scope completed','May contain dynamically materialized scopes.'),
   (90,'ready','Logical library ready state','Derived from the FSM; no separate state file.') ON CONFLICT DO NOTHING;

INSERT INTO buildengine.dom_security_query_status VALUES
   (10,'not-scanned','Version has not yet been checked',''),
   (20,'scanned','Version has a complete recorded security check','A zero-CVE result is meaningful only here.'),
   (30,'incomplete','Not every required source could be evaluated',''),
   (40,'failed','Version security evaluation failed','') ON CONFLICT DO NOTHING;

CREATE INDEX IF NOT EXISTS ix_library_scope_state_current ON
   buildengine.library_scope_state(library_id,version_no,state_id,completed_at);

CREATE OR REPLACE VIEW buildengine.v_library_runtime_cache AS
SELECT v.library_id,v.version_no,v.version_text,
       v.package_present,v.documentation_present,
       v.sbom_sha256,v.sbom_path,
       v.sbom_document IS NOT NULL AS sbom_available,
       v.license_info_document IS NOT NULL AS license_info_available,
       v.artifact_manifest IS NOT NULL AS artifact_manifest_available,
       v.cache_checked_at,
       ss.status_code AS security_status,
       sec.checked_at AS security_checked_at,sec.diagnostic_summary AS security_diagnostic
FROM buildengine.library_version AS v
LEFT JOIN buildengine.version_security_status AS sec
   USING(library_id,version_no)
LEFT JOIN buildengine.dom_security_query_status AS ss
   ON ss.status_id=sec.status_id;

CREATE OR REPLACE VIEW buildengine.v_library_state_cache AS
SELECT sc.library_id,sc.version_no,sc.state_id,d.state_code,
       COUNT(*) AS expected_scopes,
       COUNT(sc.completed_at) AS completed_scopes,
       bool_and(sc.completed_at IS NOT NULL) AS is_complete,
       CASE WHEN bool_and(sc.completed_at IS NOT NULL)
            THEN MAX(sc.completed_at) ELSE NULL END AS completed_at,
       MAX(sc.checked_at) AS last_checked_at
FROM buildengine.library_scope_state AS sc
JOIN buildengine.dom_library_state AS d USING(state_id)
GROUP BY sc.library_id,sc.version_no,sc.state_id,d.state_code;

GRANT SELECT ON ALL TABLES IN SCHEMA buildengine TO buildengine_reader;

ALTER DEFAULT PRIVILEGES IN SCHEMA buildengine GRANT SELECT ON TABLES TO buildengine_reader;

-- EXISTING-SCHEMA-UPGRADE-END

-- BuildEngine CVE assessment XML import for existing PostgreSQL installations.
-- Run once as buildengine_user after the baseline has been provisioned.
-- No SQLite dependency. No schema/version identifier is imposed on the XML.
-- Each invocation is a PostgreSQL transaction; errors roll back the batch.
-- Historical judgments are bound to a specific build contract timestamp.

ALTER TABLE buildengine.cve_assessment
   ADD COLUMN IF NOT EXISTS evaluated_contract_timestamp timestamptz;

-- PostgreSQL expands SELECT * when a view is created; refresh the projection
-- after the newly added contract column, preserving existing view identity.
CREATE OR REPLACE VIEW buildengine.v_cve_assessment_current AS
SELECT DISTINCT ON (library_id,version_no,cve_id) *
FROM buildengine.cve_assessment
ORDER BY library_id,version_no,cve_id,changed_at DESC;

CREATE OR REPLACE FUNCTION buildengine.import_cve_assessments_xml(str_document text)
RETURNS bigint
LANGUAGE plpgsql
SECURITY INVOKER
SET search_path = pg_catalog, buildengine
AS $assessment_import$
DECLARE
   the_document xml;
   the_record record;
   i_library bigint;
   i_version bigint;
   i_cve bigint;
   i_applicability integer;
   i_exposure integer;
   i_risk integer;
   i_decision integer;
   i_reason integer;
   i_publication integer;
   the_existing record;
   i_modified bigint := 0;
   i_affected bigint;
   the_timestamp timestamptz;
   the_contract_timestamp timestamptz;
   str_operation text;
BEGIN
   IF str_document IS NULL OR btrim(str_document) = '' THEN
      RAISE EXCEPTION 'Assessment XML is empty';
   END IF;
   the_document := xmlparse(document str_document);
   IF NOT xpath_exists('/assessments',the_document) THEN
      RAISE EXCEPTION 'Expected XML root <assessments>';
   END IF;

   FOR the_record IN
      SELECT *
      FROM xmltable('/assessments/assessment' PASSING the_document
         COLUMNS library_code text PATH '@library',
                 version_text text PATH '@version',
                 cve_code text PATH '@cve',
                 changed_at text PATH '@changedAt',
                 operation_text text PATH '@operation',
                 contract_timestamp text PATH '@contractTimestamp',
                 reviewer text PATH '@reviewer',
                 applicability text PATH '@applicability',
                 exposure text PATH '@exposure',
                 effective_risk text PATH '@effectiveRisk',
                 decision text PATH '@decision',
                 reason text PATH '@reason',
                 publication text PATH '@publication',
                 comment_text text PATH 'comment',
                 evidence_text text PATH 'evidence')
   LOOP
      str_operation := COALESCE(NULLIF(btrim(the_record.operation_text),''),'Standard');
      IF str_operation NOT IN ('Standard','Replace','Delete') THEN
         RAISE EXCEPTION 'Unknown assessment operation: %',str_operation;
      END IF;
      IF the_record.library_code IS NULL OR the_record.version_text IS NULL
         OR the_record.cve_code IS NULL OR the_record.changed_at IS NULL THEN
         RAISE EXCEPTION 'Assessment identity (library, version, CVE, changedAt) is required';
      END IF;
      IF the_record.cve_code !~ '^CVE-[0-9]{4}-[0-9]{4,}$' THEN
         RAISE EXCEPTION 'Invalid CVE code: %',the_record.cve_code;
      END IF;
      the_timestamp := the_record.changed_at::timestamptz;
      IF the_timestamp > clock_timestamp() THEN
         RAISE EXCEPTION 'Future-dated assessment is not accepted: %',the_record.changed_at;
      END IF;
      SELECT l.library_id,v.version_no INTO i_library,i_version
      FROM buildengine.library AS l
      JOIN buildengine.library_version AS v USING(library_id)
      WHERE l.library_code=the_record.library_code
        AND v.version_text=the_record.version_text;
      IF i_library IS NULL THEN
         RAISE EXCEPTION 'Unknown library/version: %@%',
            the_record.library_code,the_record.version_text;
      END IF;

      IF str_operation='Delete' THEN
         -- Delete is idempotent and never inserts a CVE or a provider finding.
         -- A missing assessment is already in the requested final state.
         DELETE FROM buildengine.cve_assessment AS a
         USING buildengine.cve AS c
         WHERE a.library_id=i_library AND a.version_no=i_version
           AND a.cve_id=c.cve_id AND c.cve_code=the_record.cve_code
           AND a.changed_at=the_timestamp;
         GET DIAGNOSTICS i_affected=ROW_COUNT;
         i_modified:=i_modified+i_affected;
         CONTINUE;
      END IF;

      IF the_record.contract_timestamp IS NULL
         OR the_record.reviewer IS NULL OR btrim(the_record.reviewer)=''
         OR the_record.comment_text IS NULL OR btrim(the_record.comment_text)=''
         OR the_record.evidence_text IS NULL OR btrim(the_record.evidence_text)='' THEN
         RAISE EXCEPTION 'Standard/Replace assessment requires contract, reviewer, comment and evidence';
      END IF;
      the_contract_timestamp:=the_record.contract_timestamp::timestamptz;
      -- Old contract reviews may be imported but are stale in the live reader.
      SELECT value_id INTO i_applicability FROM buildengine.dom_applicability
         WHERE code=the_record.applicability;
      SELECT value_id INTO i_exposure FROM buildengine.dom_exposure
         WHERE code=the_record.exposure;
      SELECT value_id INTO i_risk FROM buildengine.dom_effective_risk
         WHERE code=the_record.effective_risk;
      SELECT value_id INTO i_decision FROM buildengine.dom_decision
         WHERE code=the_record.decision;
      SELECT value_id INTO i_reason FROM buildengine.dom_reason
         WHERE code=the_record.reason;
      SELECT value_id INTO i_publication FROM buildengine.dom_publication_state
         WHERE code=the_record.publication;
      IF i_applicability IS NULL OR i_exposure IS NULL OR i_risk IS NULL
         OR i_decision IS NULL OR i_reason IS NULL OR i_publication IS NULL THEN
         RAISE EXCEPTION 'Invalid assessment reference domain for %@% %',
            the_record.library_code,the_record.version_text,the_record.cve_code;
      END IF;

      IF str_operation='Standard' THEN
         -- Provider findings and the historical version/CVE relation survive
         -- assessment updates and deletes.
         INSERT INTO buildengine.cve(cve_code,title)
         VALUES(the_record.cve_code,'Awaiting independent provider record')
         ON CONFLICT(cve_code) DO NOTHING;
         SELECT cve_id INTO i_cve FROM buildengine.cve
            WHERE cve_code=the_record.cve_code;
         INSERT INTO buildengine.version_cve(library_id,version_no,cve_id,evidence)
         VALUES(i_library,i_version,i_cve,
            jsonb_build_object('origin','assessment-xml',
               'assessmentChangedAt',the_timestamp::text)::text)
         ON CONFLICT(library_id,version_no,cve_id) DO NOTHING;
      ELSE
         -- Replace is an exceptional correction of an existing assessment:
         -- do not create an otherwise unknown provider or version-CVE link.
         SELECT a.cve_id INTO i_cve FROM buildengine.cve_assessment AS a
         JOIN buildengine.cve AS c USING(cve_id)
         WHERE a.library_id=i_library AND a.version_no=i_version
           AND c.cve_code=the_record.cve_code AND a.changed_at=the_timestamp;
         IF i_cve IS NULL THEN
            RAISE EXCEPTION 'Replace target assessment not found: %@% % %',
               the_record.library_code,the_record.version_text,
               the_record.cve_code,the_timestamp;
         END IF;
      END IF;

      SELECT applicability_id,exposure_id,effective_risk_id,decision_id,
             reason_id,publication_state_id,reviewer,comment,evidence,
             evaluated_contract_timestamp
      INTO the_existing
      FROM buildengine.cve_assessment
      WHERE library_id=i_library AND version_no=i_version AND cve_id=i_cve
        AND changed_at=the_timestamp;

      IF FOUND THEN
         IF ROW(the_existing.applicability_id,the_existing.exposure_id,
                the_existing.effective_risk_id,the_existing.decision_id,
                the_existing.reason_id,the_existing.publication_state_id,
                the_existing.reviewer,the_existing.comment,the_existing.evidence,
                the_existing.evaluated_contract_timestamp)
            IS NOT DISTINCT FROM
            ROW(i_applicability,i_exposure,i_risk,i_decision,i_reason,i_publication,
                the_record.reviewer,the_record.comment_text,the_record.evidence_text,
                the_contract_timestamp) THEN
            CONTINUE; -- identical content always remains idempotent
         END IF;
         IF str_operation='Standard' THEN
            RAISE EXCEPTION 'Conflicting assessment at %@% % %; use Replace explicitly',
               the_record.library_code,the_record.version_text,
               the_record.cve_code,the_timestamp;
         END IF;
         UPDATE buildengine.cve_assessment
         SET applicability_id=i_applicability,exposure_id=i_exposure,
             effective_risk_id=i_risk,decision_id=i_decision,reason_id=i_reason,
             publication_state_id=i_publication,reviewer=the_record.reviewer,
             comment=the_record.comment_text,evidence=the_record.evidence_text,
             evaluated_contract_timestamp=the_contract_timestamp,
             recorded_at=clock_timestamp()
         WHERE library_id=i_library AND version_no=i_version AND cve_id=i_cve
           AND changed_at=the_timestamp;
         i_modified:=i_modified+1;
         CONTINUE;
      END IF;

      IF str_operation='Replace' THEN
         RAISE EXCEPTION 'Replace target assessment not found: %@% % %',
            the_record.library_code,the_record.version_text,
            the_record.cve_code,the_timestamp;
      END IF;
      INSERT INTO buildengine.cve_assessment(
         library_id,version_no,cve_id,changed_at,reviewer,applicability_id,
         exposure_id,effective_risk_id,decision_id,reason_id,publication_state_id,
         comment,evidence,evaluated_contract_timestamp)
      VALUES(i_library,i_version,i_cve,the_timestamp,the_record.reviewer,
         i_applicability,i_exposure,i_risk,i_decision,i_reason,i_publication,
         the_record.comment_text,the_record.evidence_text,the_contract_timestamp);
      i_modified:=i_modified+1;
   END LOOP;
   RETURN i_modified;
END;
$assessment_import$;
-- Explicit function grant: the reader never gains XML import privileges.
REVOKE ALL ON FUNCTION buildengine.import_cve_assessments_xml(text) FROM PUBLIC;
GRANT EXECUTE ON FUNCTION buildengine.import_cve_assessments_xml(text)
   TO buildengine_user;

COMMIT;
