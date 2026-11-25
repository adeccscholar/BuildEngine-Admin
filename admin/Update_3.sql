-- BuildEngine PostgreSQL Update_3: explicit view JOIN contracts.
-- Apply to an existing database after Update_2.sql as buildengine_user.
-- Greenfield databases use BuildEngine.sql instead.
BEGIN;

CREATE OR REPLACE VIEW buildengine.v_library_versions AS
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
JOIN buildengine.library_version AS v ON v.library_id=l.library_id
JOIN buildengine.dom_category AS cat ON cat.category_id=l.category_id
JOIN buildengine.dom_license_signal AS sig
   ON sig.license_signal_id=COALESCE(v.license_signal_id,l.license_signal_id)
LEFT JOIN buildengine.dom_artifact_type AS atype
   ON atype.artifact_type_id=v.artifact_type_id;

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
   ON sec.library_id=v.library_id AND sec.version_no=v.version_no
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
JOIN buildengine.dom_library_state AS d ON d.state_id=sc.state_id
GROUP BY sc.library_id,sc.version_no,sc.state_id,d.state_code;

CREATE OR REPLACE VIEW buildengine.v_catalog_sync_history AS
SELECT s.*,d.status_code,d.description AS status_description
FROM buildengine.catalog_sync AS s
JOIN buildengine.dom_catalog_sync_status AS d ON d.status_id=s.status_id;

CREATE OR REPLACE VIEW buildengine.v_cve_max_severity AS
SELECT c.cve_id,c.cve_code,MAX(r.cvss_score) AS max_cvss,
       MAX(r.epss_score) AS max_epss
FROM buildengine.cve AS c
LEFT JOIN buildengine.cve_rating AS r ON r.cve_id=c.cve_id
GROUP BY c.cve_id,c.cve_code;

GRANT SELECT ON ALL TABLES IN SCHEMA buildengine TO buildengine_reader;
COMMIT;
