# Central Library Security Assessment

At this documented stage, BuildEngine has exactly **one** assessment level: a centralized security assessment of the specific libraries built by BuildEngine.

## Central working database

The programs share:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

It stores languages, localized reference values, provider findings, build/library findings and centralized assessment revisions.

`BuildEngine-Manager` reads and writes this database directly. `BuildEngine-Server` opens the same file in read-only mode.

There is currently **no** second working database under `build`, no separate local assessment tier and no application-specific assessment.

## Repository as the distribution source

The versioned reference database lives in `BuildEngine-Admin` at:

```text
admin\security\build-assessments.sqlite
```

A fresh installation or standard repository synchronization transfers this file into the working directory. When the repository contains an updated copy, the normal Admin synchronization **may and should overwrite** the existing file at the production path. That is how centrally maintained languages, value ranges, provider findings and library assessments are distributed.

Consequently, **do not mark this database as `preserve`** in repository synchronization.

## Central editing workflow

Assessments are edited centrally in:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

The Manager can import new CVE/provider findings from monitoring, create new library assessments, update existing assessments as new revisions, and use or eventually administer localized languages and value ranges.

After reviewing a change, explicitly copy the working database to the checked-out Admin repository:

```text
<RepositoriesRoot>\BuildEngine-Admin\admin\security\build-assessments.sqlite
```

Perform normal Git review, commit and push. The new repository version then becomes the shared reference and is distributed to other BuildEngine installations by the normal Admin sync.

## Distribution cycle

```text
BuildEngine-Admin repository
   admin\security\build-assessments.sqlite
        |
        | RepositorySync
        v
<ProductionRoot>\admin\security\build-assessments.sqlite
        |
        | BuildEngine-Manager: monitoring + assessment
        v
<ProductionRoot>\admin\security\build-assessments.sqlite
        |
        | explicitly copy for distribution
        v
<RepositoriesRoot>\BuildEngine-Admin\admin\security\build-assessments.sqlite
        |
        | Git review / commit / push
        v
BuildEngine-Admin repository
```

## Current assessment scope

Assess only the concrete BuildEngine library configuration, for example: an affected feature disabled during the build; a vulnerable component not compiled or not shipped; an exploit prerequisite absent from the chosen build profile; a local patch applied; or a newer selected version containing the fix.

Application-specific claims are outside this model and belong to a later extension.
