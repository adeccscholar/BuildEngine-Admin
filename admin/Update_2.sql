-- BuildEngine PostgreSQL Update_2: XML assessment operation semantics.
-- Apply to an existing installation after Update_1.sql.
-- Standard (including absent attribute) appends dated decisions.
-- Replace corrects an existing dated judgment; Delete removes just that judgment.
-- No CVE, version-CVE or provider-rating record is deleted.
BEGIN;

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

REVOKE ALL ON FUNCTION buildengine.import_cve_assessments_xml(text) FROM PUBLIC;
GRANT EXECUTE ON FUNCTION buildengine.import_cve_assessments_xml(text)
   TO buildengine_user;

COMMIT;
