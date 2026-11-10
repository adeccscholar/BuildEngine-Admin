# BuildEngine Tool Contracts

This page summarizes tool definitions from `build-tools.xml` and `build-source-tools.xml`. **Provisioning** distinguishes **mapped** (existing SDK or host tools), **provisioned** (verified tool packages), and **source-built** (BuildEngine's own source-tool FSM). These labels describe the declared integration approach, not an observed successful tool build.

| Tool | Version / Pin | Provisioning |
|---|---|---|
| C++Builder / bcc64x | 20.1.7 | mapped |
| CMake | 4.1.1 | mapped |
| Windows Resource Compiler | Windows SDK | mapped |
| embarcadero-make | 5.43 | mapped |
| llvm-ar / llvm-ranlib / ld-lld | 20.1.7 | mapped |
| bcc64x-windres | 20.1.7 | mapped |
| tdump | RAD Studio 13 | mapped |
| Git / Bash | 2.55.0.3 / 5.2 | provisioned |
| Ninja | 1.13.2 | provisioned |
| Python | 3.14.7 | provisioned |
| Strawberry Perl | 5.42.2.1 | provisioned |
| Meson | 1.12.0 | provisioned |
| Doxygen | 1.18.0 | provisioned |
| Graphviz | 15.1.1 | provisioned |
| NASM | 3.02 | provisioned |
| winflexbison | 2.5.24 | provisioned |
| pkg-config | 0.28-1 | provisioned |
| fonttools | 4.65.0 | provisioned |
| llvm-cov | 20.1.7 | provisioned |
| Mermaid | 11.17.2 | provisioned |
| Highlight.js | 11.12.0 | provisioned |
| MathJax | 3.2.2 | provisioned |
| GN (former) | b2afae122eeb6ce09c52d63f67dc53fc517dbdc8 | mapped (Skia; transitional) |
| GN (planned) | 07d3c6f4dc290fae5ca6152ebcb37d6815c411ab | source-built (not yet built in this historical contract) |

Additional tools are maintained exclusively in their XML contracts. This Markdown overview captures a **configuration snapshot**, not the live ToolRegistry state. The final server page should derive all three categories directly from XML and runtime evidence rather than permanently duplicating this table.

## Independently built tools

The intended lifecycle is **Source/Patch → Prepare → Release → Smoke → Publish**, with persistent evidence under `.buildengine/source-tools`, independent of library FSMs. See [GN](gn.md).
