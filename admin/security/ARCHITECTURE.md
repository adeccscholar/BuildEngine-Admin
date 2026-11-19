# Security Assessment Architecture: XML Exchange and PostgreSQL

**19 November 2026.** XML is the sole communications and exchange format for
library/CVE assessments. PostgreSQL maintains authoritative operating state,
date-ordered history and provider evidence. The former SQLite process is obsolete.

## Ownership and trust boundaries

1. **Upstream providers** independently supply CVE descriptions, applicability
   ranges, CVSS, EPSS, KEV and observations. Their original data must not be
   overwritten by local judgments.
2. **BuildEngine-Admin** distributes `admin/security/assessments.xml`, the
   reviewable, transportable assessment exchange document. There is no required
   XML schema-version property at this stage.
3. **BuildEngine-Server** reads that XML from the synchronized Production Root
   at startup and uses `PostgresCveStore::ImportAssessmentXml` and the
   PostgreSQL function `buildengine.import_cve_assessments_xml(text)`.
4. **PostgreSQL** retains original `cve`, `version_cve`, `cve_rating` and
   date-keyed `cve_assessment` rows.
5. **BuildEngine-Common** reads this state through
   `PostgresCatalogReader`. The Server shows original provider severity
   and assessment-derived risk, each with its own provenance and chronology.
6. **Future human assessment editor** modifies PostgreSQL through a managed
   workflow and exports the same XML structure. The editor does not introduce
   any alternative data exchange mechanism.

## Assessment identity

A decision is identified by:

- logical library ID and exact original library version;
- CVE identifier;
- date/time with UTC offset (`changedAt`);
- reviewer;
- applicability, exposure, effective risk, decision, reason and publication;
- technical explanation and evidence.

A fixed upstream release can retain a CVE association with decision
`fixed-by-update`. A locally patched original version can instead use
`patched-locally`, provided that the actual patch and evidence are named.
A disabled feature uses `fixed-by-configuration` only when its exact build
switch and the vulnerability's prerequisites are known. Missing runtime
proof must not be silently rewritten into 'not applicable'.

## Import behavior

The importer resolves the exact library/version from the catalog and CVE
code by identity, never from a filesystem path or display name. All rows
are inserted within the calling transaction; unknown coordinates,
invalid domain codes and equal-timestamp conflicting assessments fail
the import. Replaying identical XML produces zero new rows. A new later
`changedAt` generates a new historical decision, leaving earlier rows
available. No provider score or past assessment row is deleted.

The SQL migration for existing PostgreSQL databases is located at
`BuildEngine-Server/internal/postgresql/assessment-import.sql`, and
the same function is included in the greenfield `buildengine.sql`.

## Deployment and validation gates

- The PostgreSQL structure and exact library catalog must exist before importing.
- Admin sync must copy `assessments.xml` to the Production Root.
- The reader and writer roles remain separate. The HTTP server never
  exposes a public write endpoint for assessment imports.
- Re-run the import after source patches or build-contract changes and
  create new assessments only when evidence and conclusions warrant revision.
- Test duplicate imports, same-date conflicts, invalid coordinates,
  old-version retention, local-patch evidence and browser presentation.
- Native BCC64X compilation, PostgreSQL execution and complete HTTP tests
  have **not** yet been independently performed for this new functionality.

The first review provider is OpenAI GPT-6, with operator review before
production acceptance. This is genuine engineering evidence analysis,
not fictional, synthetic 'toy' security data.
