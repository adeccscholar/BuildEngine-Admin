from pathlib import Path
import re
import xml.etree.ElementTree as ET

main_path = Path("admin/build-libraries.xml")
fragment_dir = Path("admin/build-libraries.d")
text = main_path.read_text(encoding="utf-8")
text = re.sub(r'schemaVersion="\d+"', 'schemaVersion="13"', text, count=1)
closing = "</buildLibraries>"
if text.count(closing) != 1:
    raise SystemExit("canonical build-libraries.xml must contain exactly one closing root tag")

blocks = []
for path in sorted(fragment_dir.glob("*.xml")):
    fragment = path.read_text(encoding="utf-8")
    start = fragment.find("<library ")
    end = fragment.rfind("</library>")
    if start < 0 or end < 0:
        raise SystemExit(f"no library block in {path}")
    blocks.append(fragment[start:end + len("</library>")].rstrip())

if not blocks:
    raise SystemExit("no library fragments found to consolidate")

prefix, _ = text.split(closing)
main_path.write_text(prefix.rstrip() + "\n" + "\n".join(blocks) + "\n" + closing + "\n",
                     encoding="utf-8", newline="\n")

for path in fragment_dir.glob("*.xml"):
    path.unlink()
fragment_dir.rmdir()

readme_path = Path("README.md")
readme = readme_path.read_text(encoding="utf-8")
readme_replacement = """## Library contract organization

`admin/build-libraries.xml` is the single normative library graph and package contract. Library definitions are not split into XML fragments. Supporting CMake adapters, patches, smoke sources and helper programs remain separate implementation assets under `admin/`, but every library id, version and dependency is visible in the one canonical XML file."""
readme, count = re.subn(r"## Modular library contract files\n.*?(?=\n## |\Z)", readme_replacement, readme, flags=re.S)
if count:
    readme_path.write_text(readme.rstrip() + "\n", encoding="utf-8", newline="\n")

todo_path = Path("TODO.md")
todo = todo_path.read_text(encoding="utf-8")
todo_replacement = """## BuildEngine-Vertragsorganisation

`admin/build-libraries.xml` ist der einzige normative Bibliotheks- und Dependency-Vertrag. Aktive Bibliotheksdefinitionen werden nicht auf XML-Fragmente verteilt. CMake-Adapter, Patches, Smoke-Quellen und Hilfsprogramme bleiben als technische Assets unter `admin/` getrennt, waehrend IDs, Versionen, Varianten und der gesamte Dependency-DAG in einer XML-Datei sichtbar und pruefbar bleiben."""
todo, count = re.subn(r"## BuildEngine-Vertragsorganisation\n.*?(?=\n## |\Z)", todo_replacement, todo, flags=re.S)
if count:
    todo_path.write_text(todo.rstrip() + "\n", encoding="utf-8", newline="\n")

root = ET.parse(main_path).getroot()
ids = [node.attrib["id"] for node in root.findall("library")]
required = {"xerces-c", "ace-tao", "bzip2", "glew", "opengl", "raylib", "sdl2", "sqlite"}
missing = sorted(required.difference(ids))
if missing:
    raise SystemExit("missing consolidated libraries: " + ", ".join(missing))
duplicates = sorted({value for value in ids if ids.count(value) > 1})
if duplicates:
    raise SystemExit("duplicate library ids: " + ", ".join(duplicates))
if root.attrib.get("schemaVersion") != "13":
    raise SystemExit("schemaVersion is not 13")
if fragment_dir.exists():
    raise SystemExit("fragment directory still exists")

Path(".github/workflows/consolidate-library-contract.yml").unlink()
Path(".github/scripts/consolidate_library_contract.py").unlink()
print(f"validated {len(ids)} libraries in canonical contract")
