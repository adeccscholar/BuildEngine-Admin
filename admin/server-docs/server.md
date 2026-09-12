# BuildEngine Server

[TOC|Content]

BuildEngine Server is the read-only HTTP presentation and machine-interface layer for a BuildEngine production tree. It exposes library/package metadata, generated documentation, PDF documentation, SBOM data, dependency and usage information, security results, package export, project documentation, and a browser UI. It does not replace the BuildEngine scheduler or technical step-state model.

Related project documentation:

- [BuildEngine architecture](buildengine.md)
- [Configuration and CLI](configuration.md)
- [Documentation contract](documentation.md)
- [Tool contract](build-tools.md)
- [Library contract](build-libraries.md)
- [Tool overview](tools.md)

## One production tree, one repository view

The server works directly on the central BuildEngine production tree. It does not maintain a server-specific copy of libraries, packages, generated documentation, SBOMs, security assessments, or administration metadata.

`BuildEngineRepository` is the shared repository abstraction over that production tree. The console engine, server, manager, and other front ends can therefore consume the same centrally produced state instead of reconstructing it independently.

```mermaid
flowchart LR
   Contracts["Synchronized Admin contracts"] --> Production["Central production tree"]
   Engine["BuildEngine"] --> Production
   Production --> Repository["BuildEngineRepository"]
   Repository --> Server["Server"]
   Repository --> Manager["Manager"]
   Repository --> Other["Other front ends"]
```

This strengthens the BuildEngine principle of **one truth**: XML contracts describe intended state, BuildEngine produces and verifies the effective artifacts and evidence, and presentation layers read that same central state. The HTTP server is a projection of the repository, not an independent state authority.

## Transport and scope

The server deliberately uses HTTP for its current presentation and machine interface. This is a design decision for the intended deployment model, not a limitation of the underlying networking stack.

The safe defaults remain local:

```text
serverAddress="127.0.0.1"
serverName="localhost"
serverPort="8765"
```

The endpoint is configured centrally in `BuildEngine.xml`:

```xml
<parameters
   ...
   serverAddress="127.0.0.1"
   serverName="localhost"
   serverPort="8765"
   .../>
```

`serverAddress` is the concrete local IP interface on which the listener binds. `serverName` is the logical DNS/server identity accepted in the HTTP `Host` header and used when presenting the endpoint. `serverPort` selects the TCP port.

A protected-network deployment can bind to one explicitly selected internal interface and expose the corresponding internal server name. Wildcard listener addresses such as `0.0.0.0` or `::` are intentionally rejected: the deployment contract should identify the interface that is intended to be reachable.

If a non-loopback interface is selected, responsibility for keeping that endpoint behind the appropriate firewall and network boundary belongs to the deployment environment.

HTTP is appropriate for that deliberately constrained environment because introducing HTTPS would also introduce certificate provisioning, trust configuration, renewal/rotation, expiration handling, and operational ownership. Those concerns are intentionally kept outside this project's current scope.

The server architecture is not coupled to plain HTTP. The Boost.Asio/Beast based transport can be extended to HTTPS without changing the repository, rendering, REST, package, or security service layers. Such an extension would primarily add TLS transport and certificate configuration. The absence of HTTPS should therefore be understood as a scope and operational-responsibility decision, not as an architectural restriction.

Only HTTP `GET` requests are accepted by the current server implementation. The browser UI and REST-style JSON endpoints share the same server.

The standalone server reads these values from a central `BuildEngine.xml` when it is supplied with `--config` or when `BuildEngine.xml` is present in the working directory. Explicit `--root`, `--address`, `--name`, and `--port` arguments are runtime overrides rather than a second configuration authority.

## Browser routes

| Route | Purpose |
| --- | --- |
| `/server/` | Server dashboard and information area |
| `/status/` | Effective server status |
| `/libraries/` | Configured and installed library overview |
| `/library/{library}/` | Versions of one library |
| `/library/{library}/{version}/` | Library/version detail |
| `/pdf/{library}/{version}` | Published PDF documentation for a library version |
| `/packages/` | Installed package overview |
| `/package/{library}/{version}/` | Package detail and export link |
| `/security/` | Security overview |
| `/security/{library}/{version}/` | Security analysis detail |
| `/manual/story.md` | Project story and engineering motivation |
| `/manual/buildengine.md` | BuildEngine architecture |
| `/manual/server.md` | This server and REST API document |
| `/manual/configuration.md` | Configuration and CLI reference |
| `/manual/documentation.md` | Documentation, Doxygen, LaTeX and PDF contract |
| `/manual/build-tools.md` | `build-tools.xml` contract reference |
| `/manual/build-libraries.md` | `build-libraries.xml` contract reference |
| `/manual/tools.md` | Current BuildEngine tool overview |
| `/manual/images/...` | Synchronized project-documentation images, including nested directories |
| `/index.html` | Generated central library documentation index |

Static generated documentation is served from the central BuildEngine documentation root after the explicit application routes have been evaluated.

## Published PDF documentation

When the documentation pipeline has produced a PDF for a library/version, the server exposes it through:

```text
/pdf/{library}/{version}
```

The route resolves the published PDF from the same central documentation tree used by BuildEngine and sends it with MIME type:

```text
application/pdf
```

The server deliberately does not force a `Content-Disposition: attachment` header for this route. Modern browsers can therefore display the PDF directly and provide their normal download/save function. This also keeps PDF delivery consistent with the general principle that generated documentation is served directly from the central BuildEngine output rather than copied into a server-specific area.

## Request flow

```mermaid
flowchart LR
   B[Browser or internal client] --> H[HTTP listener]
   H --> V{Configured interface and host?}
   V -- no --> F[Rejected request]
   V -- yes --> R{Route type}
   R -->|Machine API| J[JSON response]
   R -->|Dashboard/UI| P[Rendered HTML page]
   R -->|Project Markdown| M[cmark-gfm]
   M --> A[Feature analysis and TOC prereader]
   A --> P
   R -->|Markdown image| I[admin/server-docs/images]
   R -->|Published PDF| PDF[documentation/.../pdf]
   R -->|Generated documentation| S[File response]
   R -->|Browser assets| W[Managed /js resource]
```

## Machine API

The machine API accepts both `/api/...` and the versioned `/api/v1/...` form for the common library endpoints. New consumers should prefer the versioned form.

### Status

```http
GET /api/v1/status HTTP/1.1
Host: localhost:8765
Accept: application/json
```

Returns application/version information, configured server identity, and the effective production/documentation roots.

### Libraries

```http
GET /api/v1/libraries
GET /api/v1/libraries/{library}
GET /api/v1/libraries/{library}/{version}
GET /api/v1/libraries/{library}/{version}/dependencies
GET /api/v1/libraries/{library}/{version}/findings
GET /api/v1/libraries/{library}/{version}/usage
```

The first endpoint lists installed package libraries. The following endpoints progressively expose versions, one concrete version, direct dependencies, security findings, and reverse usage information.

### Library, SBOM, and risk resources

The server also exposes direct versioned resource routes:

```http
GET /api/v1/library/{library}/{version}
GET /api/v1/sbom/{library}/{version}
GET /api/v1/risk/{library}/{version}
```

The SBOM response is the installed CycloneDX document. The risk endpoint combines provider findings with the shared assessment model from BuildEngine-Common.

### Package export

```http
GET /api/v1/package/{library}/{version}/download
```

The package exporter creates the requested package archive from the installed BuildEngine package information and returns it as a download response.

## Example response

A status response has the following conceptual shape:

```json
{
  "application": "BuildEngineDocServer",
  "version": "...",
  "api": "...",
  "apiBase": "/api",
  "versionedApiBase": "/api/v1",
  "bind": "127.0.0.1",
  "serverName": "localhost",
  "port": 8765,
  "productionRoot": "D:/local/embarcadero/test_v3",
  "documentationRoot": "D:/local/embarcadero/test_v3/documentation",
  "transport": "HTTP on explicitly configured interface"
}
```

Values shown with ellipses depend on the running server build.

## Request dispatch

The HTTP server evaluates machine routes first, then browser and documentation resources. The important separation is visible in simplified form below:

```cpp
if(WriteMachineApi(theSocket, theRequest, vecSegments)) {
   // JSON machine interface handled.
}
else if(/* /js/... managed browser asset */) {
   // Serve managed JavaScript/CSS resource.
}
else if(/* /manual/images/... synchronized image */) {
   // Serve an image from admin/server-docs/images.
}
else if(/* /pdf/{library}/{version} */) {
   // Serve the published PDF from the central documentation tree.
}
else if(/* explicit browser route */) {
   // Render dashboard, libraries, packages or security page.
}
else {
   // Resolve project Markdown or generated documentation.
}
```

The implementation uses modern C++23 throughout the project and favors value semantics, `std::string_view`, `std::filesystem`, ranges, structured bindings, `std::optional`, and other modern facilities over older pointer-heavy interfaces where practical.

## Markdown rendering pipeline

Project documentation is maintained directly in the synchronized Admin repository below `admin/server-docs`. A request for `/manual/*.md` resolves to that synchronized source file and renders its current contents on demand. There is no second manually maintained Markdown copy below the generated documentation tree.

```mermaid
sequenceDiagram
   participant Admin as admin/server-docs
   participant Browser
   participant Server
   participant Renderer as MarkdownRenderer
   participant CMark as cmark-gfm
   participant Assets as Managed browser assets

   Browser->>Server: GET /manual/server.md
   Server->>Admin: resolve current Markdown source
   Server->>Renderer: RenderFile(...)
   Renderer->>Renderer: Analyze features and TOC directive
   Renderer->>CMark: Parse + render GFM
   CMark-->>Renderer: HTML body
   Renderer->>Renderer: Inject generated anchors, TOC and back links
   Renderer-->>Server: HTML + feature mask
   Server-->>Browser: Complete page
   Browser->>Server: Request selected /js assets
   Server->>Assets: Resolve registered tool roots
   Assets-->>Browser: JavaScript/CSS
```

The Markdown source is inspected for browser capabilities required by the page. Language-marked source blocks enable syntax highlighting, Mermaid diagrams enable Mermaid, and supported mathematical delimiters enable MathJax. Only the required browser resources are emitted for each rendered page.

### Mermaid presentation

Mermaid diagrams are rendered centrally by the server and are intentionally limited to approximately 82 percent of the Markdown content width. Individual documents therefore do not need size-specific Mermaid markup. Oversized diagrams remain scrollable, while ordinary diagrams no longer dominate the page width.

### Generated table of contents

The Markdown prereader recognizes a BuildEngine-specific directive outside fenced code blocks:

```markdown
[TOC|Content]
```

The text after `TOC|` is the visible title, so `[TOC|Overview]` and `[TOC|Table of Contents]` are valid as well.

The prereader assigns deterministic document-unique anchors to headings following the directive, renders a nested list according to the heading hierarchy, creates an anchor before the table of contents itself, and adds `Back to <title>` before each heading on the shallowest section level. Project documents place the directive immediately after their `#` document title, making `##` sections the top-level TOC entries.

The generated anchors are injected after cmark-gfm has rendered safe HTML. The feature therefore does not require enabling arbitrary raw HTML in Markdown source.

## Images in project Markdown

Project-documentation images are stored in the Admin repository below:

```text
admin/server-docs/images/
```

Subdirectories are supported. A Markdown file can reference an image with a normal relative Markdown path:

```markdown
![BuildEngine architecture](images/architecture/buildengine-overview.png)
```

When the page is served below `/manual/`, the browser resolves the reference to `/manual/images/architecture/buildengine-overview.png`. The server maps that route only to the synchronized `admin/server-docs/images` tree and rejects path traversal. Supported browser image formats are SVG, PNG, JPEG, GIF, and WebP.

The normal Admin repository synchronization transfers the complete image tree recursively into the production Admin tree. Images follow exactly the same SHA-256 content rule as the other synchronized Admin files: a missing image is copied, an image with the same SHA-256 remains unchanged, and an image whose content hash differs is replaced. File timestamps do not determine image synchronization.

This keeps synchronization deterministic and avoids a second documentation-copy or state mechanism.

## Links between Markdown documents

Relative Markdown links are supported and are the preferred way to connect project documentation:

```markdown
[Tool overview](tools.md)
[Tool contract](build-tools.md)
[Library contract](build-libraries.md)
```

When the current document is served as `/manual/documentation.md`, the browser resolves `tools.md` to `/manual/tools.md`. The normal server fallback then resolves the corresponding file from `admin/server-docs` and renders it live.

This keeps project documentation relocatable inside the `/manual/` namespace and avoids hard-coded host names or ports.

## Managed browser resources

Stable browser URLs are grouped below `/js`:

```text
/js/highlighting/...
/js/mermaid/...
/js/mathjax/...
```

The server maps these stable URLs to the currently registered managed tool roots. HTML pages therefore do not need to know the installed tool version or physical directory.

| Feature | Browser resource | Loaded when |
| --- | --- | --- |
| Highlight.js | `/js/highlighting/...` | A language-marked code fence exists |
| Mermaid | `/js/mermaid/...` | A `mermaid` fence exists |
| MathJax | `/js/mathjax/...` | Supported math delimiters are detected |

## Project and generated documentation

The manually authored project documents are synchronized with the Admin repository:

```text
admin/server-docs/story.md
admin/server-docs/buildengine.md
admin/server-docs/server.md
admin/server-docs/configuration.md
admin/server-docs/documentation.md
admin/server-docs/build-tools.md
admin/server-docs/build-libraries.md
admin/server-docs/tools.md
admin/server-docs/images/**
```

They are read and rendered directly from that synchronized tree. All public-facing project Markdown in this live set is maintained in English; original-language titles may remain in parentheses when identifying referenced works.

Generated per-library HTML and PDF documentation remains below the normal BuildEngine documentation root and is served by the same HTTP server. This gives the running server one entry point for both evolving project documentation and generated third-party library documentation without mixing their source locations or creating another copy.

The cmark-gfm runtime is validated when the server is initialized. A missing renderer runtime therefore fails visibly instead of leaving apparently available Markdown routes that cannot be rendered.

## Documentation maintenance rule

The project Markdown files are part of the implementation contract. Code, XML schema and Markdown documentation are maintained together.

Examples:

- a new `build-tools.xml` construct also updates [build-tools.md](build-tools.md),
- a new or changed tool also updates [tools.md](tools.md),
- a new `build-libraries.xml` construct also updates [build-libraries.md](build-libraries.md),
- documentation-pipeline changes also update [documentation.md](documentation.md),
- HTTP, endpoint configuration, rendering, PDF delivery, TOC preprocessing, image handling, or link behavior changes also update this document.

Documentation is therefore not a release-afterthought; it is maintained as part of the same change that modifies the corresponding behavior.

## Shared service layer

The server deliberately does not duplicate repository, security, package, or rendering logic. These capabilities live in BuildEngine-Common and can be consumed by the console application and the VCL manager as well.

That separation makes the HTTP server a presentation surface rather than a second domain implementation. For example, the risk assessment exposed as HTML and JSON is the same assessment model available to the other BuildEngine front ends.

Most importantly, the server creates its `BuildEngineRepository` directly from the configured central production root. Package metadata, SBOMs, generated documentation, PDFs, security assessments, tool state, and synchronized Admin content therefore remain shared evidence. The server does not reinterpret those artifacts into a second persistent store.

## Security boundary

The intended security boundary is a deliberately restricted network endpoint rather than TLS termination inside BuildEngine itself.

- The default uses the loopback interface and `localhost`.
- A protected-network deployment may use one explicitly selected internal IP address and server name.
- Wildcard bind addresses are rejected; the listener must identify a concrete interface.
- The configured server name or interface address is validated against the HTTP `Host` header.
- A non-loopback deployment remains the operator's responsibility and must stay behind the intended firewall or equivalent network boundary.
- HTTP is an explicit design choice for this constrained deployment model, not a technical limitation.
- HTTPS can be added at the transport layer if a deployment requires it, but certificate lifecycle and trust management are intentionally outside the present project scope.
- Documentation path traversal is rejected.
- Managed browser asset paths are constrained.
- Manual image paths are constrained to `admin/server-docs/images` and accepted image formats.
- Machine endpoints validate library coordinates through the repository layer.
- The server is read-only with respect to BuildEngine build state.

## Related documentation

Use the relative links at the top of this page to navigate through the project documentation. Generated library documentation starts at `/index.html`.
