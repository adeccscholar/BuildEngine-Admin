# BuildEngine Security Assessments

This directory contains the **centralized, distributable assessment of the actual BuildEngine library and build configuration**. Application-specific assessments are not yet part of the documented design.

## One shared database

The central assessment uses one SQLite file:

```text
admin\security\build-assessments.sqlite
```

In the working BuildEngine environment it is synchronized to:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

The same database stores provider findings, BuildEngine/library findings, assessment revisions, languages, localized reference values, a review queue and views.

There is **no** second database solely for reference values and **no** separate working database under `build`. The `schema.sql` and `reference-data.sql` files are readable bootstrap and migration definitions for the same SQLite database.

## Repository as distribution source

The reference copy is versioned at:

```text
BuildEngine-Admin\admin\security\build-assessments.sqlite
```

Normal repository synchronization copies it to:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

When the repository copy changes, it **may and should overwrite** the working copy to distribute centrally maintained languages, value ranges, provider findings and library assessments. The file must **not** be marked as `preserve`.

## Central editing

`BuildEngine-Manager` reads and writes the working database, importing new CVE/provider findings, updating library findings, recording assessment revisions and loading languages and localized values from that same file.

`BuildEngine-Server` opens the same file strictly read-only and displays the centrally distributed assessments.

## Publishing reviewed assessments

After a reviewed central change, deliberately copy the database back to the checked-out Admin repository:

```text
<RepositoriesRoot>\BuildEngine-Admin\admin\security\build-assessments.sqlite
```

Perform Git review, commit and push. Only then does the updated version become the reference distributed to other BuildEngine working environments.

## Language handling

Languages have stable identifiers in `language`. Reference values also have stable IDs; their localized texts live in `reference_value_text`.

The Server reads its default language from `BuildEngine.xml`, for example:

```xml
<parameters serverLanguage="de" ... />
```

The Manager uses `managerLanguage`, falling back to English (`en`) if absent. Both codes resolve through `language.code`. German (`id=1`, `code=de`) and English (`id=2`, `code=en`) are initially present.

**UI localization is separate from the external documentation policy:** all public Markdown documentation is written in English.

## Current assessment scope

The assessment is limited to a concrete library/build configuration, such as: an affected feature disabled at build time; a vulnerable component not compiled or not deployed; exploit prerequisites absent from the selected profile; a local fix; or a newer version including a fix. Application-specific assertions are outside the current assessment level.

## Versioned companion files

- `schema.sql`: canonical SQLite database schema
- `reference-data.sql`: initial languages and localized applicability, exposure, effective risk, decision and justification values
- `build-assessments.sqlite`: versioned centralized assessment database

Schema and reference values remain in the repository alongside the binary database so the structure, migrations and reviews can be inspected.

## Principles

- Never overwrite provider severity.
- `no finding` does not mean `safe`.
- Bind assessments to the library, version, upstream identity/commit and build evidence.
- When evidence changes, previous assessments must become visibly stale.
- Store revisions; never destructively replace historical decisions.
- Automatically place unknown findings in the review queue.
- `not applicable` is not equivalent to `false positive`.
- The central database may contain only build-configuration assessments.
