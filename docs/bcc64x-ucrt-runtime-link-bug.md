# BCC64X 20.1.7: fehlerhafte UCRT-Symbolauflösung bei Win64

Status: **reproduziert und durch Gegenprobe bestätigt**  
Toolchain: RAD Studio 13 Florence / BCC64X 20.1.7 / Win64 Modern / LLD 20.1.7

## Kurzfassung

BCC64X erzeugt bei einer echten, adressierbaren Referenz auf bestimmte mathematische C-Runtime-Funktionen einen PE-Import aus `api-ms-win-crt-math-l1-1-0.dll`. Die DLL wird erfolgreich gelinkt, kann unter Windows jedoch nicht geladen werden (`ERROR_PROC_NOT_FOUND`, Fehler 127).

Die mit RAD Studio ausgelieferte `x86_64-w64-mingw32/lib/libucrt.a` enthält für dieselben Symbole zusätzlich lokale `libucrt_extra`-Implementierungen. Werden diese Objekte explizit vor der normalen Runtime gelinkt, verschwinden die problematischen API-Set-Imports und `LoadLibrary` funktioniert.

Der Fehler wurde zunächst durch Skia Debug sichtbar, ist aber **nicht Skia-spezifisch**.

## Betroffene Symbole

In der untersuchten `libucrt.a` existieren für genau folgende Symbole sowohl ein API-Set-Anbieter als auch eine lokale `libucrt_extra`-Implementierung:

- `fabsf`
- `nextafterl`
- `nexttoward`
- `nexttowardf`
- `nexttowardl`

Dazu gehören vier lokale Objekte:

- `lib64_libucrt_extra_a-fabsf.o`
- `lib64_libucrt_extra_a-nextafterl.o` (liefert auch `nexttowardl`)
- `lib64_libucrt_extra_a-nexttoward.o`
- `lib64_libucrt_extra_a-nexttowardf.o`

## Minimaler Reproducer

Quelle: `admin/programs/bcc64x/ucrt-runtime-probe.c`

Build ohne Workaround:

```cmd
"C:\Program Files (x86)\Embarcadero\Studio\37.0\bin64\bcc64x.exe" -tD ucrt-runtime-probe.c -o ucrt-runtime-probe.dll
```

Importtabelle:

```cmd
tdump -em ucrt-runtime-probe.dll | findstr /i "fabsf nextafterl nexttoward"
```

Beobachtetes Ergebnis:

```text
IMPORT: api-ms-win-crt-math-l1-1-0.dll='fabsf'
IMPORT: api-ms-win-crt-math-l1-1-0.dll='nextafterl'
IMPORT: api-ms-win-crt-math-l1-1-0.dll='nexttoward'
IMPORT: api-ms-win-crt-math-l1-1-0.dll='nexttowardf'
IMPORT: api-ms-win-crt-math-l1-1-0.dll='nexttowardl'
```

`LoadLibrary` schlägt anschließend mit Fehler 127 (`Die angegebene Prozedur wurde nicht gefunden`) fehl.

## Evidenz aus `libucrt.a`

Für `fabsf` zeigt `llvm-nm --print-armap` gleichzeitig:

```text
__imp_fabsf in api-ms-win-crt-math-l1-1-0.dll
fabsf in api-ms-win-crt-math-l1-1-0.dll
...
lib64_libucrt_extra_a-fabsf.o:
00000000 T fabsf
```

Die normale Link-Map enthält andere `libucrt_extra`-Objekte, aber nicht `lib64_libucrt_extra_a-fabsf.o`. Das fertige PE importiert stattdessen das API-Set-Symbol.

Die genaue interne LLD-Auswahlregel (z. B. erster Archive-Map-Treffer) ist **[nicht verifiziert]**. Verifiziert ist dagegen, welcher Anbieter im fertigen PE verwendet wird und dass die lokale Implementierung im normalen Link nicht extrahiert wird.

## Positive Gegenprobe

Die vier genannten Objekte wurden aus derselben, lokal installierten `libucrt.a` extrahiert und zu `libbcc64x-ucrt-compat.a` zusammengefasst. Derselbe Reproducer wurde anschließend mit diesem Archiv vor der normalen Runtime gelinkt.

Ergebnis:

- keiner der fünf problematischen API-Set-Imports ist mehr vorhanden;
- die DLL linkt erfolgreich;
- `LoadLibrary` liefert `LOAD OK`.

Damit ist die Fehlerursache unterhalb der Bibliotheksebene reproduziert.

## BuildEngine-Workaround

Der Workaround wird nicht als Skia-Quellpatch und nicht über ein PowerShell-Hilfsskript umgesetzt. Die vollständige Provisioning-Logik liegt im BuildEngine-Toolvertrag.

`admin/build-tools.xml` deklariert ein generiertes Tool `bcc64x-ucrt-compat`. Der Vertrag enthält:

- das lokal installierte BCC64X-`llvm-ar` als Archiver,
- die lokal installierte `x86_64-w64-mingw32/lib/libucrt.a` als Quellarchiv,
- genau die vier verifizierten `libucrt_extra`-Members,
- das versionierte Zielverzeichnis unter dem BuildEngine-Tools-Root,
- `libbcc64x-ucrt-compat.a` als registrierten Tool-Entry.

BuildEngine erzeugt dieses Archiv selbst. Existiert der versionierte Tool-Entry bereits und ist er nicht älter als die Quell-`libucrt.a`, wird keine Generierung ausgeführt. Andernfalls extrahiert BuildEngine die deklarierten Members mit `llvm-ar`, prüft die extrahierten Dateien, erzeugt das Compat-Archiv in einem temporären Verzeichnis und promotet es erst nach erfolgreichem Abschluss in den endgültigen Toolpfad.

Damit wird das Archiv einmal pro Toolversion und Produktionsumgebung bereitgestellt und anschließend wiederverwendet. Es werden keine Embarcadero-Objektdateien oder Archive im Repository gespeichert.

Der generische CMake-BCC64X-Vertrag konsumiert nur das bereits bereitgestellte Tool und hängt dessen Archiv vor die normalen Runtime-Bibliotheken. CMake selbst erzeugt kein Compat-Archiv mehr.

Der Workaround gilt für Win64-BCC64X-Linkvorgänge allgemein und ist nicht auf Debug beschränkt. Debug machte den Fehler lediglich sichtbar, weil Optimierung bei `fabsf` häufig eine externe Referenz vermeidet.

## Vorschlag für Embarcadero-Bugreport (Englisch)

**Title:** BCC64X 20.1.7 libucrt.a resolves x64 math compatibility symbols to non-loadable API-set imports

**Description:**

On RAD Studio 13 / BCC64X 20.1.7, a Win64 DLL that takes the address of `fabsf`, `nextafterl`, `nexttoward`, `nexttowardf`, or `nexttowardl` links successfully but fails to load with Windows error 127 (`ERROR_PROC_NOT_FOUND`).

The generated PE imports these symbols from `api-ms-win-crt-math-l1-1-0.dll`.

The shipped `x86_64-w64-mingw32/lib/libucrt.a` contains both API-set definitions for these symbols and local `libucrt_extra` implementations. For example, `llvm-nm --print-armap libucrt.a` reports both an API-set `fabsf` definition and `lib64_libucrt_extra_a-fabsf.o` with a real `T fabsf` definition.

During the normal BCC64X DLL link, the local compatibility object is not extracted and the resulting DLL contains the API-set import. If the already shipped `lib64_libucrt_extra_a-fabsf.o` is linked explicitly, the API-set import disappears and the DLL loads successfully. The same result is obtained for the complete five-symbol set by linking the four corresponding `libucrt_extra` objects.

This reproduces independently of Skia or any third-party library.

**Expected:** the normal BCC64X runtime link selects the supplied x64 `libucrt_extra` implementations where required, producing a loadable Win64 binary.

**Actual:** LLD selects/emits the API-set imports and the resulting DLL fails at runtime with `ERROR_PROC_NOT_FOUND`.
