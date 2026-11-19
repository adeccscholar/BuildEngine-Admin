-- Update_1.sql: first additive update to the existing BuildEngine PostgreSQL catalog.
-- Includes the previously required catalog/runtime changes and XML assessment import.
-- Apply ONLY to an existing BuildEngine schema; use BuildEngine.sql for new installations.
-- Do not run after BuildEngine.sql on a fresh database.
BEGIN;

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
   i_imported bigint := 0;
   the_timestamp timestamptz;
   str_evidence text;
   the_contract_timestamp timestamptz;
   the_current_contract_timestamp timestamptz;
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
                 contract_timestamp text PATH '@contractTimestamp',
                 changed_at text PATH '@changedAt',
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
      IF the_record.library_code IS NULL OR the_record.version_text IS NULL
         OR the_record.cve_code IS NULL OR the_record.changed_at IS NULL
         OR the_record.contract_timestamp IS NULL
         OR the_record.reviewer IS NULL OR btrim(the_record.reviewer) = ''
         OR the_record.comment_text IS NULL OR btrim(the_record.comment_text) = ''
         OR the_record.evidence_text IS NULL OR btrim(the_record.evidence_text) = '' THEN
         RAISE EXCEPTION 'Missing required identity, reviewer, comment or evidence in assessment row';
      END IF;
      IF the_record.cve_code !~ '^CVE-[0-9]{4}-[0-9]{4,}$' THEN
         RAISE EXCEPTION 'Invalid CVE code: %', the_record.cve_code;
      END IF;
      the_timestamp := the_record.changed_at::timestamptz;
      the_contract_timestamp := the_record.contract_timestamp::timestamptz;
      IF the_timestamp > clock_timestamp() THEN
         RAISE EXCEPTION 'Future-dated assessment is not accepted: %',the_record.changed_at;
      END IF;
      SELECT l.library_id,v.version_no,v.current_contract_timestamp
      INTO i_library,i_version,the_current_contract_timestamp
      FROM buildengine.library l
      JOIN buildengine.library_version v USING(library_id)
      WHERE l.library_code=the_record.library_code
        AND v.version_text=the_record.version_text;
      IF i_library IS NULL THEN
         RAISE EXCEPTION 'Unknown library/version: %@%',the_record.library_code,the_record.version_text;
      END IF;
      -- An assessment for an older contract remains importable as history.
      -- The reader flags it stale against the current contract; it must not
      -- silently neutralize a vulnerability in the new build.
      SELECT value_id INTO i_applicability FROM buildengine.dom_applicability WHERE code=the_record.applicability;
      SELECT value_id INTO i_exposure FROM buildengine.dom_exposure WHERE code=the_record.exposure;
      SELECT value_id INTO i_risk FROM buildengine.dom_effective_risk WHERE code=the_record.effective_risk;
      SELECT value_id INTO i_decision FROM buildengine.dom_decision WHERE code=the_record.decision;
      SELECT value_id INTO i_reason FROM buildengine.dom_reason WHERE code=the_record.reason;
      SELECT value_id INTO i_publication FROM buildengine.dom_publication_state WHERE code=the_record.publication;
      IF i_applicability IS NULL OR i_exposure IS NULL OR i_risk IS NULL
         OR i_decision IS NULL OR i_reason IS NULL OR i_publication IS NULL THEN
         RAISE EXCEPTION 'Invalid assessment reference domain for %@% %',
            the_record.library_code,the_record.version_text,the_record.cve_code;
      END IF;
      -- Provider vulnerability facts are never overwritten by reviewer data.
      INSERT INTO buildengine.cve(cve_code,title)
         VALUES(the_record.cve_code,'Awaiting independent provider record')
         ON CONFLICT(cve_code) DO NOTHING;
      SELECT cve_id INTO i_cve FROM buildengine.cve WHERE cve_code=the_record.cve_code;
      -- The CVE-to-version link is retained for historical fixes and non-applicability.
      INSERT INTO buildengine.version_cve(library_id,version_no,cve_id,evidence)
         VALUES(i_library,i_version,i_cve,
            jsonb_build_object('origin','assessment-xml',
               'assessmentChangedAt',the_timestamp::text)::text)
         ON CONFLICT(library_id,version_no,cve_id) DO NOTHING;
      str_evidence := the_record.evidence_text;
      SELECT applicability_id,exposure_id,effective_risk_id,decision_id,reason_id,
             publication_state_id,reviewer,comment,evidence,evaluated_contract_timestamp
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
            IS DISTINCT FROM
            ROW(i_applicability,i_exposure,i_risk,i_decision,i_reason,i_publication,
                the_record.reviewer,the_record.comment_text,str_evidence,
                the_contract_timestamp) THEN
            RAISE EXCEPTION 'Conflicting assessment at %@% % %',
               the_record.library_code,the_record.version_text,the_record.cve_code,the_timestamp;
         END IF;
         CONTINUE;
      END IF;
      INSERT INTO buildengine.cve_assessment(
         library_id,version_no,cve_id,changed_at,reviewer,applicability_id,
         exposure_id,effective_risk_id,decision_id,reason_id,publication_state_id,
         comment,evidence,evaluated_contract_timestamp)
      VALUES(i_library,i_version,i_cve,the_timestamp,the_record.reviewer,
         i_applicability,i_exposure,i_risk,i_decision,i_reason,i_publication,
         the_record.comment_text,str_evidence,the_contract_timestamp);
      i_imported := i_imported+1;
   END LOOP;
   RETURN i_imported;
END;
$assessment_import$;

-- Explicit function grant: the reader never gains XML import privileges.
REVOKE ALL ON FUNCTION buildengine.import_cve_assessments_xml(text) FROM PUBLIC;
GRANT EXECUTE ON FUNCTION buildengine.import_cve_assessments_xml(text)
   TO buildengine_user;

COMMIT;
