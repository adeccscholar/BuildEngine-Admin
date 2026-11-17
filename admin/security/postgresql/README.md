# PostgreSQL CVE persistence — staged integration

## Authority and separation

The BuildEngine Server presents **read-only CVE snapshots** to HTTP clients. The background scanner collects OSV, NVD, EPSS and CISA KEV evidence independently and publishes an immutable memory snapshot by atomic pointer replacement. Requests do not trigger scans, database updates, or per-visitor logging.

SQLite remains an **independently managed assessment archive**. This CVE schema neither attaches nor queries SQLite. A future assessment adapter may import an assessment snapshot using a short-lived read-only connection, without waiting on an archive writer or acquiring exclusive locks. The HTTP request path must not open that archive.

The first PostgreSQL migration is [001-cve-cache.sql](001-cve-cache.sql). It has three key concerns:

- `cve_snapshot`: durable scan generation, completion state and provenance.
- `cve_finding`: indexed CVE findings and enrichment, including CVSS, EPSS, percentile, KEV and remediation; immutable within a completed snapshot.
- `active_snapshot`: transactionally activated generation; its view exposes only complete findings.

## Planned BCC64X integration

The existing BuildEngine packages `libpq` 18.6 and `libpqxx` 8.0.2 are the intended C and native C++ client libraries respectively. PostgreSQL itself is a separately deployed server, not embedded in the HTTP service. No MSVC/MinGW fallback, alternative ABI or new C++ wrapper for existing C++ interfaces is required.

1. Enable a separately configured database writer in the **background collector** with a dedicated PostgreSQL role; disable it by default until a connection exists.
2. Complete OSV/NVD/EPSS/KEV fetching outside a transaction; never hold a database transaction during network I/O.
3. Insert a candidate generation and bulk-copy findings using the dedicated write connection. Only a complete, successful result may update the active generation in one transaction.
4. After the transaction commits, publish the corresponding immutable memory snapshot. On PostgreSQL failure, retain the previous active generation and existing memory snapshot.
5. On restart, load only the active complete generation into memory, then serve HTTP without further PostgreSQL reads for those responses.
6. Retire historical generations in a separate, bounded maintenance task; never delete the active generation.
7. A future assessment import is its **own** process. It may read immutable assessment evidence, but never writes to or serializes the SQLite archive.

## Required configuration and operations

- The database connection and credentials must not be stored in BuildEngine-Admin or exposed through Server APIs, web pages, traces or application startup logs.
- Configure TLS or a dedicated protected PostgreSQL network connection, least-privilege roles, authentication, statement/lock timeouts and connection limits.
- Use one bounded writer pool (initially one writer), a separate read/recovery connection, explicit transaction ownership and parameterized queries.
- The in-memory snapshot remains the normal read path; PostgreSQL is for restart durability, history, and multi-process consistency, not per-request queries.
- Validate schema, migrations, rollback, indexing and query plans against the deployed PostgreSQL 18 server before enabling it.

## Implementation and verification status

**Committed:** initial SQL schema and atomic, SQLite-independent in-memory CVE publication.

**Not yet active:** PostgreSQL connection, native `libpqxx` consumer code, database migrations on a live server, restart reload, assessment archive import, throughput tests and failover testing. These require native BCC64X and PostgreSQL integration evidence.

This is intentionally non-invasive: no BuildEngine library timestamp is changed, no existing archive is migrated, and no PostgreSQL service is silently started.
