# PostgreSQL-backed BuildEngine catalog: simplified greenfield model

## Scope

The **planned** central server uses PostgreSQL in the local database `BuildEngine` (default localhost:5432). All tables, value lists, foreign keys and views are defined in **one** file: [BuildEngine-Server/internal/postgresql/buildengine.sql](https://github.com/adeccscholar/BuildEngine-Server/blob/main/internal/postgresql/buildengine.sql). Superseded `001-buildengine.sql`, `002-security.sql` and `verify-baseline.sql` have been deleted.

**[not verified]** PostgreSQL deployment and server read-path migration have not yet been executed.

## Normalized library metadata

`library.library_id bigint` is the primary identity; `library.library_code` is the unique natural key. General library name, description, supplier, homepage, category, license signal and license notice are stored **once** in `library`, not duplicated into each version.

`library_version` is identified by `(library_id, version_no)` and also enforces `UNIQUE(library_id,version_text)`. The row holds a genuinely version-specific description, optional license notice and alternate license signal, artifact type, runtime DLL, license evidence, security repository identity, SBOM reference and the raw `components_manifest text` from the versioned `bundled-components.json`. The central server may evaluate the manifest on demand.

Original/current XML contract timestamps are separate from created/modified database timestamps. Source categories not yet present in `dom_category` must be inserted when encountered; descriptions and notes can be supplied separately. The domain tables `dom_category`, `dom_license_signal` and `dom_artifact_type` have real foreign keys and documentation fields.

## Library relationships

`library_relation` is both the association and its dated history. No relation surrogate ID, extension-shared-path table or separate revision table is necessary. The composite primary key includes exact source library/version, target library and effective date. Target version, relationship type and presence are the state recorded at that effective date. The newest effective relationship replaces the older state in the default server display, with the full chronological data available through a history view.

`dom_relation_type` includes `dependency`, `extended`, **`supersededBy`**, `direct`, `transitive`, and `bundled`. Technical build sharing paths are kept in BuildEngine's source/build manifests rather than duplicated into the relational server model.

## XML import logging

`catalog_sync` records each import attempt of the entire `build-libraries.xml` with source SHA-256, input counts, change counts, start/end times and `diagnostic_summary`. The integer `status_id` references `dom_catalog_sync_status` (running 10, complete 20, partial 30, failed 40). There is no `catalog_generation` or active-generation pointer. CVE lifecycles do not depend on the XML import log.

## CVE and assessment model

`cve` stores the independent global CVE identity and the latest narrative. Updated or expanded title, description or references can be written in place; these texts do **not** require historization.

`version_cve` links individual library versions directly to CVEs. No separate component entity is required. `cve_rating` retains a new dated row only when a provider assessment actually changes (e.g. CVSS score, vector, EPSS or KEV). The server displays the historical maximum CVSS via `v_cve_max_severity` and can display the rating timeline.

`cve_assessment` records changes in the six previously established assessment dimensions: applicability, exposure, effective risk, decision, reason and publication state. Each dimension has its own integer `dom_` value list and strict foreign key. The row also holds a reviewer, date, comment and evidence. Unchanged evaluations are not inserted again.

Provider scan attempts, copies of whole JSON documents, component inventories and additional revisions were intentionally omitted as unnecessary for the initial server data model.

## Database roles and service access

The single SQL baseline provisions two login roles without embedded passwords: `buildengine_user` owns the `buildengine` schema and its tables/views, can read/write data, create and alter schema objects, and is intended for BuildEngine and administrative UI execution. `buildengine_reader` has only database connection, schema usage, and `SELECT` permission on the current and future catalog tables/views, and is intended for the eventual Windows server service identity.

Default read-only grants are applied to future tables/views created by `buildengine_user`. The script must be executed by a PostgreSQL administrator with role-creation privileges. Windows service identity mapping and integrated SSPI authentication are configured separately in PostgreSQL, not by committing credentials to this repository. The roles are **not** independently verified against a deployed server.

## Windows integrated user security

Authentication and permissions are managed separately. **Windows / Active Directory** authenticates the actual BuildEngine operator or Windows service process identity through PostgreSQL **SSPI**. PostgreSQL's `pg_ident.conf` then maps that authenticated Windows principal to an explicitly requested PostgreSQL login role. Finally, schema ownership, foreign keys and SQL `GRANT` permissions control the role's actions. The connection string includes a database role, **not** a Windows username or password.

- **BuildEngine and trusted desktop administration:** an explicitly approved Windows principal maps to `buildengine_user`; this is the schema owner with create/alter/write rights.
- **BuildEngine-Server Windows service:** a dedicated Windows service account maps to `buildengine_reader`; it can select from permitted current and future tables/views but cannot modify data or schema.

The PostgreSQL administrator configures restrictive `pg_hba.conf` SSPI rules for `BuildEngine`, the two database role names and approved client addresses; `pg_ident.conf` maps only explicitly approved Windows principals to these roles. Keep realm identification enabled and respect the principal format returned by the local Windows/AD environment. When deployed across the network, also configure the PostgreSQL server's Kerberos SPN, restricted client networks and transport security, then verify authentication behavior. SSPI may negotiate NTLM instead of Kerberos, so the mere presence of SSPI configuration is not proof of Kerberos use.

When an operator or service account changes, update both the Windows identity/AD permissions and the PostgreSQL identity mapping. Do not give the server account membership in the writer role. Future table read permissions are inherited from `ALTER DEFAULT PRIVILEGES` **only for tables created by `buildengine_user`**, so additional schema creators require deliberate permission management.

For connection examples, complete `pg_hba.conf`/`pg_ident.conf` templates, role tests and administrator maintenance procedures, see the [BuildEngine-Server PostgreSQL README](https://github.com/adeccscholar/BuildEngine-Server/blob/main/internal/postgresql/README.md#2-configure-postgresql-authentication-rules). PostgreSQL roles alone do not provision Windows accounts, and the SQL script deliberately embeds no passwords. **[not verified]** Real Windows service account login and SSPI negotiation remain to be tested against the target server.

## Planned runtime integration

BuildEngine remains responsible for reading XML and synchronizing the database using `adecc-core-lib`, `defined_type_list`, `Persistent_Systemdata`, and the existing PostgreSQL libpq/libpqxx package. The server is to read database data exclusively, retaining filesystem access for documents, logs and installed artifacts. No unrelated C++ compiler ecosystem or ORM is introduced.

The single SQL baseline replaces the earlier draft without trying to migrate its unused schema. Its actual deployment and the new C++ data access require verification.
