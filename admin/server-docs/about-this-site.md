# About This Site: Live BuildEngine View and Documentation Provenance

[TOC|Content]

## The primary purpose: a live view of the BuildEngine environment

The BuildEngine Server is first and foremost the **direct, read-only visualization of the BuildEngine working environment**. It presents the logical library catalog, library versions and dependencies, build and installation status, package contents, runtime artifacts, license information, CycloneDX SBOMs, security findings, and other available operational evidence from the configured BuildEngine production environment.

This information is read from the BuildEngine repository and production data available to the server, rather than recreated as a manually maintained website or inferred from a fixed documentation snapshot. The declarative contracts provide the configured library identities and relationships; installed packages, generated metadata and persisted state provide the corresponding operational evidence. A library declared in XML is not necessarily installed or successfully tested. Likewise, a recorded earlier successful run is not proof for a later revision of the contract.

The server is a **presentation and inspection layer**, not a second build orchestrator, a separate library-state authority, or an independent source of package truth. Common BuildEngine services resolve logical library identities, including extensions and shared physical packages.

## HTML and Markdown documentation

In addition to showing operational data, the server can deliver existing **HTML files** and display **Markdown (`.md`) documentation** directly in the browser. Markdown is a convenient and widely used format for technical project documentation because it remains readable and editable as text, works naturally in source repositories, and supports review and revision history.

The Markdown renderer supports:

- **Syntax highlighting** for fenced source-code examples.
- **Mermaid** diagrams for architecture, control flow and dependency graphs.
- **MathJax** for mathematical expressions and equations.
- Headings, links, tables, and document navigation, including the project's table-of-contents directive.

These features are used to render the existing documentation, not to replace its underlying Markdown source with a separately maintained HTML copy. Presentation depends on the rendering and browser assets available in the deployed server environment.

## Where the documentation comes from

The written documentation reflects the technical direction, requirements and decisions supplied by the project owner. Its source material includes the owner's own templates, architectural specifications, technical guidance, articles and blog posts, as well as project notes and explanations provided during development.

A further important source is the **actual course of the BuildEngine project**: its evolving XML contracts, C++ source changes, version-specific patches, dependency decisions, tests, build diagnostics, issue investigations, revisions and commit history. The engineering history records not only successful results, but also failures, corrections, rejected approaches and changing implementation states where those are documented.

The owner determines the project's goals, technical constraints and architecture. AI assistance does not replace those decisions or become an independent authority over them.

## How AI is used

**AI-assisted documentation** means that artificial intelligence has been used to collect, correlate, organize, draft and update parts of these texts from the available source material and engineering history, including repository revisions where accessible.

AI helps turn a large volume of technical material into coherent library guides, architectural explanations, dependency diagrams, change summaries and a navigable historical account. It may also help compare documentation with the current declared contract and identify inconsistencies for correction.

The distinction between **source material, editorial preparation and verified engineering evidence** is essential:

- The owner's originals, instructions and recorded decisions remain the source of the intended project direction.
- The versioned XML, source code, patches and other repository artifacts define the implementation state at a specific revision.
- Actual build logs, installed binaries, executed tests and observed runtime results establish what has been verified.
- AI-generated descriptions, summaries, diagrams and historical reconstructions are editorial representations of those sources. They are **not independent evidence** that a library builds, loads or works.

Where the available material is incomplete or does not establish a claim, the documentation should state that limitation explicitly, for example with **[not verified]**. Editorial synthesis must not convert an intention, an unexecuted configuration or a historical result into a current PASS claim.

## Two different kinds of information

| Live environment view | Project documentation |
| --- | --- |
| Displays the BuildEngine data and evidence available at the time the server reads it | Explains architecture, decisions, configurations, work history and background |
| Uses the Common repository and logical catalog model | Draws on the owner's material and versioned project sources |
| Shows actual installed metadata where present | May discuss planned, historical or unverified work if clearly identified |
| Is not authored anew by AI for each request | Has been organized and edited with AI assistance |

This distinction prevents confusion between a dynamic operational dashboard and a technically informed narrative about the project.

## Attribution and revision context

**Project direction, original technical material and engineering decisions:** project owner, adecc Systemhaus GmbH.

**Documentation preparation and editorial organization:** AI-assisted, using the owner's materials, development history and available revision evidence.

**Implementation and test status:** authoritative only for the corresponding source revision, BuildEngine contract, build configuration and actual execution evidence.

The notice **AI-assisted documentation** in the shared page footer links to this explanation. It describes how the documentation is prepared; it does **not** imply that the live operational information displayed by BuildEngine Server is generated by AI.
