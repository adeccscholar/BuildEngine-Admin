# PostgreSQL-backed BuildEngine catalog: simplified greenfield model

## Scope

The optional central server catalog uses PostgreSQL in the local database `BuildEngine` (default localhost:5432). All tables, value lists, foreign keys and views are defined in **one** file: [BuildEngine-Admin/admin/buildengine.sql](https://github.com/adeccscholar/BuildEngine-Admin/blob/main/admin/buildengine.sql). Superseded `001-buildengine.sql`, `002-security.sql` and `verify-baseline.sql` have been deleted.

**[not verified]** The database-backed HTTP pages and PostgreSQL CVE persistence are implemented in source, but the final server binary has not yet been compiled or exercised against the target Windows/BCC64X/PostgreSQL deployment. The writer catalog import has been exercised by the project operator.

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

The standalone PostgreSQL provisioning script defines two login roles without embedded passwords: `buildengine_user` owns the `buildengine` schema and its tables/views, can read/write data, create and alter schema objects, and is intended for BuildEngine and administrative UI execution. `buildengine_reader` has only database connection, schema usage, and `SELECT` permission on the current and future catalog tables/views, and is intended for the eventual Windows server service identity.

Default read-only grants are applied to future tables/views created by `buildengine_user`. The script must be executed by a PostgreSQL administrator with role-creation privileges. Windows service identity mapping and integrated SSPI authentication are configured separately in PostgreSQL, not by committing credentials to this repository. The roles are **not** independently verified against a deployed server.

## Windows integrated user security

Authentication and permissions are managed separately. **Windows / Active Directory** authenticates the actual BuildEngine operator or Windows service process identity through PostgreSQL **SSPI**. PostgreSQL's `pg_ident.conf` then maps that authenticated Windows principal to an explicitly requested PostgreSQL login role. Finally, schema ownership, foreign keys and SQL `GRANT` permissions control the role's actions. The connection string includes a database role, **not** a Windows username or password.

- **BuildEngine and trusted desktop administration:** an explicitly approved Windows principal maps to `buildengine_user`; this is the schema owner with create/alter/write rights.
- **BuildEngine-Server Windows service:** a dedicated Windows service account maps to `buildengine_reader`; it can select from permitted current and future tables/views but cannot modify data or schema.

The PostgreSQL administrator configures restrictive `pg_hba.conf` SSPI rules for `BuildEngine`, the two database role names and approved client addresses; `pg_ident.conf` maps only explicitly approved Windows principals to these roles. Keep realm identification enabled and respect the principal format returned by the local Windows/AD environment. When deployed across the network, also configure the PostgreSQL server's Kerberos SPN, restricted client networks and transport security, then verify authentication behavior. SSPI may negotiate NTLM instead of Kerberos, so the mere presence of SSPI configuration is not proof of Kerberos use.

When an operator or service account changes, update both the Windows identity/AD permissions and the PostgreSQL identity mapping. Do not give the server account membership in the writer role. Future table read permissions are inherited from `ALTER DEFAULT PRIVILEGES` **only for tables created by `buildengine_user`**, so additional schema creators require deliberate permission management.

For connection examples, complete `pg_hba.conf`/`pg_ident.conf` templates, role tests and administrator maintenance procedures, see the [BuildEngine-Server PostgreSQL README](https://github.com/adeccscholar/BuildEngine-Server/blob/main/internal/postgresql/README.md#2-configure-postgresql-authentication-rules). PostgreSQL roles alone do not provision Windows accounts, and the SQL script deliberately embeds no passwords. **[not verified]** Real Windows service account login and SSPI negotiation remain to be tested against the target server.

## Authoritative build evidence and database-backed server cache

The **BuildEngine source of truth is unchanged**: `admin/build-libraries.xml` defines
the build contracts, while the filesystem's per-scope `.state` files and
the FSM's evidence checks determine whether work has completed. Database
rows never authorize, suppress or invalidate a build.

The intended startup sequence is: read the whole source XML, calculate
SHA-256 over its bytes, compare against the last completed `catalog_sync`
record, and, **only on a changed hash**, create a new synchronization attempt
and reconcile the full catalog before scheduling work. When the XML hash
is unchanged, current scope evidence still needs to be rechecked against
the filesystem; an unchanged XML is not proof that states or binaries remain.

`dom_library_state` is the numeric FSM reference domain. The
`library_scope_state` table mirrors the individual logical scope, including
variant scopes and dynamic documentation scopes, with its source contract
timestamp, state file path, `completed_at` and `checked_at`. A successful
filesystem `ScopeStateStore::Commit` precedes the cache write; the exact
completion timestamp is copied, never regenerated in PostgreSQL. Invalid,
removed and outdated scope records must be cleared by reconciliation.
`v_library_state_cache` aggregates scope completion for the server and does
not replace `LibraryProgressEvaluator`.

### Verified cache-model coverage

The code inspection also identified installed package presence, CycloneDX
SBOM data, `LICENSE-INFO.txt`, artifact manifests containing real generated
DLL/EXE/LIB paths and hashes, documentation availability, current tool
registration, and security coverage status (including a successful scan
with zero CVEs). These are now modeled with the `library_version` cache
fields, `tool_cache`, `showcase_cache` and `version_security_status`, supplementing the
existing `cve`, `version_cve`, `cve_rating` and
`cve_assessment` tables.

Raw generated HTML, binaries, production logs and downloads are **not**
duplicated into the relational catalog. The server may continue to serve
the corresponding files while catalog and cache metadata come from
PostgreSQL.

The server also discovers standalone tests and showcases through
`tests/**/showcase.json`. The `showcase_cache` table mirrors each complete
manifest as a single text field with its discovery path and timestamp,
without inventing relational identities for the contained applications.
It does not replace the original test manifests.

**Implementation status [not verified]:** The BuildEngine writer now uses
`CatalogDatabase.h` with the adecc `logical_database` PostgreSQL adapter and
compile-time `defined_type_list`/`PersistentSystemData` metadata. The importer
hashes each complete original build XML, imports on changed hashes, mirrors
the filesystem's verified FSM scopes on every run, and writes an identical
completion timestamp only **after** the authoritative scope state file was
committed. The writer caches installed SBOM/license/manifest information,
tool registration and showcase manifests.

The central server now reads database host, name, role, SSPI and security
settings from the **same** `BuildEngine.xml` through
`BuildEngine-Common`. A PostgreSQL reader is wired to the JSON endpoints for
catalog, versions, dependencies, usage, tools, states, stored SBOMs,
transitive risk and persisted CVE findings. In PostgreSQL mode the server
also renders its catalog, package, dashboard, status, showcase and security
HTML directly from SQL queries; the seven filesystem-generated overview
snapshots are not rebuilt. The PostgreSQL presentation preserves the original page chrome,
library tile/list views, filtering, sorting, package actions, expandable
security findings and transitive dependency coverage. Generated documentation
and binary artifacts remain separately served from their original files.

When server security refresh is enabled, the provider scan still runs
outside HTTP request handling. The completed result is committed through
a separate typed PostgreSQL **writer** connection before the memory
snapshot is published. This writes `cve`, `version_cve`,
`version_security_status` and append-only provider `cve_rating` changes.
CVSS candidates are retained separately for OSV, CVE.org and NVD;
EPSS and KEV use their own source identities. A repeated identical
provider value does not produce a new history row. Multiple library
versions referencing one CVE do not create additional rating revisions
within one completed scan. A scan with provider/enrichment failures has
incomplete rather than successful coverage.

The `cve_assessment` domain is reserved for explicit reviewer judgments:
the automated provider scan does not invent an applicability, exposure,
risk, decision or publication judgment. Partial scan progress uses PostgreSQL checkpoints with an incomplete
coverage status; only fully completed, successful evaluation can establish
complete coverage. Without PostgreSQL the previous filesystem/memory
compatibility behavior remains available.

The separate private C++ dependencies are rooted at `../adecc` and
`../ThirdParty/libpq`, `../ThirdParty/libpqxx` at the project root,
rather than at the package installation controlled by BuildEngine's XML.
The SQL database and writer/reader login roles require PostgreSQL
administrator provisioning; the first enabled writer run creates the
`buildengine` schema and schema contents if absent. The updated server has not yet passed a BCC64X link, SSPI writer/reader
authentication test or end-to-end HTTP/CVE history integration test.

See [configuration](configuration.md) and the
[PostgreSQL technical README](https://github.com/adeccscholar/BuildEngine-Server/blob/main/internal/postgresql/README.md)
for exact settings and known limitations.

## Active showcase links and persisted REST evidence (19 November 2026)

Showcase manifests may use the symbolic version `active`. The SQL-backed
presentation now resolves the selector against a **unique, current,
non-superseded** catalog version. It links to the concrete version while
retaining the original `active` intent in the display label. Unresolved
or ambiguous selectors are labeled explicitly instead of producing a
broken `/library/{library}/active/` link. Existing browser URLs with
`active` are independently resolved, and the versioned machine API
also accepts this selector.

The GET-only API includes `/api/v1/runs/latest` for the last
`catalog_sync` record and last mirrored `library_scope_state` check,
`/api/v1/security` for persisted direct per-version CVE and scan-status
evidence, and `/api/v1/security/{library}/{version}` for the complete
per-version direct/transitive risk projection. Scan status, diagnostic
information and check timestamps are returned without querying upstream
providers. The current `/api/v1/risk/{library}/{version}` route is
retained. See [the machine API reference](/manual/server.md).

The SQL schema currently does not contain a process execution-run
entity. A catalog synchronization time or the most recent scope check
is **not** proof of when a full BuildEngine process started, ended or
succeeded. This limitation is included in the API response.

All changes here are in shared C++ code and English documentation. No
database schema, IDE project or XML configuration files were changed.
Native BCC64X / live PostgreSQL integration remains **not verified**.

## Versioned library security summary (19 November 2026)

The PostgreSQL-backed library and package detail pages now include an
explicit **Security and vulnerability status** section ahead of the
library metadata table. The summary uses the same current PostgreSQL
security evidence and dependency closure as the detailed Security view:
direct and transitive CVE counts, highest observed technical severity,
CISA KEV status, scan gaps, affected dependency links and the current
reviewed root-version assessment. The existing full Security view,
CVE catalog, dependency information and package actions remain available.

The license suitability badge is **not** a security verdict. Technical
severity and the current reviewed risk assessment are distinct. An
incomplete scan must not be presented as a successfully verified green
security status, and known transitive CVEs require an explicit review
warning even where an affected dependency does not expose a numeric
CVSS. Reviewed root-version assessments must not be misrepresented as
review of all transitive components.

The PostgreSQL projection uses explicit column aliases for every
repeated expression or column name. This corrects incorrect supplier,
homepage and artifact-type display values and incorrect successor and
dependency coordinates caused by name-based result lookup in the typed
database API. No XML, project or database-schema changes are required.

Source implementation is committed; native BCC64X compilation and
browser verification remain **[not verified]**.

## Standalone CVE catalog and historical charts (19 November 2026)

The existing `/security/` and `/security/check/` pages retain the installed
package and version-level security views. The independent **`/cves/`**
catalog lists recorded CVEs and links to **`/cve/{CVE-ID}/`**, which displays
current provider ratings, full recorded rating chronology, CVSS and EPSS
SVG step charts, affected library versions, package-specific evidence,
and dated reviewed assessments. The charts use the timestamp at which
PostgreSQL observed a changed score. They do **not** claim that this is
the original provider publication timestamp. There is no external charting
service or provider query during an HTTP request.

Machine-readable endpoints include `/api/v1/cve/{CVE-ID}`,
`/api/v1/cve/{CVE-ID}/ratings`, and
`/api/v1/assessment/{library}/{version}/{CVE-ID}/history`.

The security overview and per-package detail now use the same persisted
security snapshot and dated library dependency graph, including
superseded library versions still referenced by a current package.
Direct and transitive findings count distinct library/version/CVE
observations in the actual closure. A package cannot be labeled green
when the closure contains known findings. Without a scorable CVSS, a
visible warning is required instead of inventing a technical severity.
Transitive findings display the affected dependency coordinates and
link to their detailed security views. **A warning is not an automated
reviewed effective-risk judgment.**

The CVE timeline and the warning behavior are implemented in source but
have not passed the native BCC64X / PostgreSQL / browser integration
test **[not verified]**. No project or XML configuration changes are required.

## Existing-schema migration (19 November 2026)

The synchronized `admin/buildengine.sql` contains an explicitly delimited
`EXISTING-SCHEMA-UPGRADE` section. On each enabled BuildEngine startup, the
writer applies that idempotent, transaction-scoped section if schema
`buildengine` already exists. The migration adds all eight columns introduced
after the initial library-version baseline (`in_catalog`, `source_file`,
`sbom_document`, `license_info_document`, `artifact_manifest`,
`package_present`, `documentation_present`, `cache_checked_at`), the six
new scope, tool, showcase and security-status domain/cache tables, their
reference values and supporting views. Original XML contract timestamps,
version identity and recorded CVE assessments are not intentionally modified.

The code no longer treats the existence of a schema as sufficient proof of
compatibility. A future incompatible schema change requires another explicit,
reviewed migration. This migration is source-reviewed but **not yet verified**
against the user's live PostgreSQL database.


### Separate writer identity for CVE collection

The HTTP PostgreSQL reader remains `buildengine_reader` and has no
write privileges. The automated CVE refresh creates its own connection
using the configured `databaseWriterUser` (normally `buildengine_user`).
This connection is instantiated only after a completed scan is available
for persistence. The writer identity must be authorized separately in
PostgreSQL. Running a writer connection inside the server process increases
the privileges available to that process: this is a deployment security
decision, not an isolation boundary. A separately isolated CVE worker
under a distinct Windows security principal would offer stronger
least-privilege separation. No password is committed to the repository.

For an optional database, a failed startup reader connection falls back to
filesystem compatibility mode. A failure to persist an otherwise completed
CVE scan must remain visible as an error and must not be silently marked
as stored in PostgreSQL.

### Library metadata reconciliation

The XML metadata parser maps `name`, `description`, `supplier`,
`homepage`, `licenseSignal`, `artifactType`, and `runtimeDll` to the
existing library/version columns. The `security/repository` attributes
`url`, `ref` and `commit` are now mapped to the version-specific
`security_repository_url`, `security_repository_ref` and
`security_repository_commit` columns.

**Changed XML hash:** Run the existing full relational catalog
reconciliation, including security repository metadata.

**Unchanged XML hash:** Perform a targeted relational metadata
reconciliation instead of skipping the database entirely. UPDATE
statements use `IS DISTINCT FROM` to avoid rewriting unchanged
records. No new `catalog_sync` record is created, and neither the
original nor the current library contract timestamps are altered.
The version number and CVE relationships remain stable.

The XML is not modified by this process. Actual BCC64X execution
and a PostgreSQL backfill verification remain **[not verified]**.

### Rating chronology

`cve_rating` uses the existing key
`(cve_id, source_id, changed_at)`. On each completed scan, compare
provider evaluation fields with the latest row for that same CVE and
provider using null-safe comparisons. Insert only if a value differs;
a successful refresh with unchanged values updates the CVE's
`last_checked_at` without making a new rating revision. CVE narrative
text remains current-state information, while `version_cve` holds
package-specific evidence and last-seen timestamps.
