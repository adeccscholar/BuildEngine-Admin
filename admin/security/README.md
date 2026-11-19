# BuildEngine Security Assessments

**Operational policy, 19 November 2026:** XML is the sole assessment interchange
and distribution format. PostgreSQL is the operative catalog and historical
assessment store. SQLite is not part of the assessment workflow.

## Shared assessment exchange

The synchronized authoritative document is:

`admin/security/assessments.xml`

Each assessment contains the exact logical library ID, original version,
CVE identifier, an offset-aware date/time (`changedAt`), the reviewer,
six assessment-domain codes, a technical justification and evidence.

The document is **real engineering assessment data**, not a fictional test.
Automated provider evidence is independent and must remain unchanged;
human-reviewed decisions (initially prepared by GPT-6 and reviewed by the
operator) are associated with the actual BuildEngine source and build
configuration. Unverified claims remain unknown and require follow-up.

No XML schema version attribute is required.

## PostgreSQL ownership

- Database: `BuildEngine`; schema: `buildengine`.
- `cve` stores global vulnerability identity and provider narrative.
- `version_cve` retains the association to every exact historical version,
  including fixed versions for which the relation documents remediation.
- `cve_rating` retains original provider CVSS, EPSS and KEV history.
- `cve_assessment` holds dated local decisions, never rewriting old ones.
- The latest decision is selected by its assessment date and shown beside
  original provider severity, not in place of it.

A CVE cannot be deleted just because the effective risk becomes `none`.
A different DLL, updated build contract, newly enabled feature or altered
patch must trigger fresh applicability review.

## Import workflow

New installs apply the complete
`BuildEngine-Server/internal/postgresql/buildengine.sql` baseline.
Existing databases apply the additive
`BuildEngine-Server/internal/postgresql/Update_1.sql` migration
once using `buildengine_user`. This creates the native
`buildengine.import_cve_assessments_xml(text)` function.

After Admin repository sync, the PostgreSQL-enabled Server starts an independent configurable 60-minute-by-default polling worker which reads
`<ProductionRoot>/admin/security/assessments.xml` and invokes that function only when the complete content has changed
using the existing typed adecc PostgreSQL connection. The import is transactional. The worker retries failed content and never deletes persisted history if the file is absent. HTTP requests read current PostgreSQL assessments without a restart. Repeated identical rows are ignored; conflicting content with
the same date/CVE/version is rejected, and previous dated decisions are retained.

Example for manual administrative import:

```sql
SELECT buildengine.import_cve_assessments_xml(
   pg_read_file('ABSOLUTE_PATH_TO_SYNCHRONIZED_ASSESSMENTS_XML')
);
```

The above `pg_read_file` requires server-file permissions and is intended
**only as a DBA example**; the application uses a bounded service-side file
read and a parameterized query instead. Avoid granting file-reading privileges
to application identities.

## Local fixes and configuration exclusions

To close a provider finding as fixed by configuration, cite the precise build
flag (such as curl's `USE_NGHTTP2=OFF`) and explain which affected feature
is excluded. A source patch requires patch path/identity, upstream fix
reference, its application stage and an installed-artifact verification gate.
An upstream fixed release uses `fixed-by-update`, rather than claiming an
unrelated local source patch.

Open questions remain open: for example, a CVE confined to DTLS is not
automatically harmless solely because the most common application traffic
is TLS. Evidence of actual build and runtime exposure is required.

## Server acceptance

The Server must show current effective risk and decision for each affected
library/version and provide complete per-CVE assessment chronology. The
underlying CVSS/EPSS/KEV chronology remains intact and independent.
Imported decisions are surfaced through PostgreSQL's current-assessment
projection. A smoke test must check import idempotency, changed historical
dates, conflict rollback, SQL-role separation and missing CVE retention.

The first repository implementation is not yet native-compiled/tested on
the Windows BCC64X host. The policy and implemented source are distinguished
from a proven deployed runtime.

## Other documents

- `assessments.xml`: the sole operational assessment interchange document.
- Future assessment-editor UI exports this exact structure; it does not
  introduce a parallel exchange format.
