# PostgreSQL-backed BuildEngine catalog: greenfield target model

## Scope

The **planned** authoritative server catalog uses a local PostgreSQL database named `BuildEngine` (default localhost:5432). The database schemas live in the `BuildEngine-Server` repository under `internal/postgresql/001-buildengine.sql` and `002-security.sql`. **[not verified]** The new native PostgreSQL importer and HTTP database-only readers are not yet implemented or tested.

The target model keeps stable BIGINT `library_id` identities and per-library BIGINT `version_no` identities. The XML library code and textual version number remain unique natural keys: `UNIQUE(library_code)` and `UNIQUE(library_id,version_text)`. These fields must never be reconstructed from sort order.

## Library metadata

Each library version retains the XML's original contract timestamp as an immutable historical value and the current XML contract timestamp separately. The database adds creation, last-modification and last-seen timestamps. New supersession, extension, dependency or security associations do not reset the version's original timestamp.

Library categories are linked to `dom_category`, which has a code, human label, description and notice. The current inventory has 16 distinct categories. The licensing traffic light is linked to `dom_license_signal`; its green/yellow/red meanings follow the XSD's licensing integration contract. The source's name, description, supplier, homepage, artifact type, runtime DLL and individual license metadata are stored by version. The security repository URL/ref/commit are version-scoped, too.

## Classified relationships

`library_relation` uses a domain code from `dom_relation_type`: `dependency`, `extended`, `supersededBy`, `direct`, `transitive`, `bundled`. `supersededBy` is an explicit edge from the older version to its exact replacement of the same library. `extended` retains shared-source/package mode and its additional extension metadata. Relationships have their own creation, modification and retirement times and optional source-version ordinal validity ranges.

## Effective-dated relationships

A library relationship is a stable association between two library identities, classified by its relation type. Each new effective-dated revision identifies the precise source and target versions. **The latest effective revision wins** in the server's default relationship display; a prior version remains accessible in the full history. The date of effective change is separate from the database ingestion timestamp.

`supersededBy` retains a distinct history for each superseded source version. Other relation types such as `dependency` and `extended` replace previous target versions for the same library pair and relation type. New versions do not silently create multiple active associations. A dated withdrawn revision removes the relationship from the current view without deleting previous observations.

The server model provides `v_library_relationships_current`, `v_library_relationships_history`, and `library_relationships_at(timestamptz)` for current and historical display.

## Security and assessments

The security model preserves globally unique CVE identities, distinct non-CVE advisory aliases, affected component identities, an exact version-to-CVE table, and time-series OSV/NVD/EPSS/KEV provider observations. It includes scan coverage, failed requests, retry evidence and expiry-aware provider caches. The server's default severity view takes the **maximum historically observed CVSS score** while separate views expose chronological scores.

Assessment history stores the six existing reference dimensions: applicability, exposure, effective risk, decision, reason and publication state, with versioned revisions, reviewer, explanation and evidence fingerprints. Historical security observations and assessment revisions are append-only.

## Catalog synchronization and reference-data status

The previous `catalog_generation` / `active_catalog_generation` design has been replaced by the simpler `catalog_sync` history. The table records one attempt to import the **entire** `admin/build-libraries.xml`, including its SHA-256, XML schema version, timestamps, counts of inserted/updated/unchanged/rejected/unprocessed records, and a human-readable `diagnostic_summary`.

`catalog_sync.status_id integer` is a foreign key to `dom_catalog_sync_status`, containing the status code, label, description, notice and ordering. The four seeded statuses are `running` (10), `complete` (20), `partial` (30) and `failed` (40). A run is `complete` when all records were processed, even if nothing changed; it is `partial` when some records were accepted but others could not be processed; `failed` is reserved for a complete failure of file import with zero accepted records.

Import timestamps and status suffice to retrieve the latest attempt and latest full success through `v_catalog_sync_latest` and `v_catalog_sync_last_complete`, without a mutable active-pointer table. The SHA-256 is indexed but **not unique** because a previously partial run can retry the identical file. Full and partial import diagnostics stay available in the history.

## Future synchronization

BuildEngine will parse and validate the XML as the import source during the FSM workflow, reconcile stable IDs and typed relations, and record its outcome with a typed synchronization status and a diagnostic summary. The HTTP server must then serve catalog/security content exclusively from PostgreSQL, retaining filesystem reads only for documentation, logs and binary artifacts. The implementation must use existing adecc-core-lib, `defined_type_list`, `Persistent_Systemdata` and managed libpq/libpqxx; no new ORM.

Source schema and read-only verification script: [BuildEngine-Server PostgreSQL baseline](https://github.com/adeccscholar/BuildEngine-Server/tree/main/internal/postgresql).
