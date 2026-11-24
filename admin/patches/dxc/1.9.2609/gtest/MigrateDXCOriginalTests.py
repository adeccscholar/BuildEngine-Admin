#!/usr/bin/env python3
"""Version-pinned DXC 1.9.2609 GoogleTest and XML row migration."""
import argparse
import re
import xml.etree.ElementTree as ET
from pathlib import Path

def Save(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    if not path.exists() or path.read_text(encoding="utf-8") != text:
        path.write_text(text, encoding="utf-8", newline="\n")

def ScopeEnd(src, start):
    depth = 0
    i = start
    while i < len(src):
        if src.startswith("//", i):
            k = src.find("\n", i)
            i = len(src) if k < 0 else k+1
            continue
        if src.startswith("/*", i):
            k = src.find("*/", i+2)
            if k < 0: raise ValueError("Unterminated C++ comment")
            i = k+2
            continue
        if src.startswith('R"', i):
            m = re.match(r'R"([^ ()\\]{0,16})\(', src[i:])
            if m:
                k = src.find(')'+m.group(1)+'"', i+m.end())
                if k < 0: raise ValueError("Unterminated raw string")
                i = k+len(m.group(1))+2
                continue
        if src[i] in "'\"":
            mark = src[i]
            i += 1
            while i < len(src):
                if src[i] == "\\": i += 2
                elif src[i] == mark:
                    i += 1
                    break
                else: i += 1
            continue
        if src[i] == "{": depth += 1
        if src[i] == "}":
            depth -= 1
            if depth == 0: return i
        i += 1
    raise ValueError("Unbalanced class braces")

def Port(srcfile):
    src = srcfile.read_text(encoding="utf-8")
    src = src.replace('#include <WexTestClass.h>', '#include "dxc/Test/WEXAdapter.h"')
    if srcfile.name == "LongVectors.cpp":
        src = src.replace("class TestClassCommon {",
                          "class TestClassCommon : public ::testing::Test {")
    if srcfile.name == "ExecutionTest.cpp":
        src = src.replace("class ExecutionTest {",
                          "class ExecutionTest : public ::testing::Test {")
    patches = []
    registrations = 0
    # Only original class definitions beginning on a source line are candidates.
    # Previously, arbitrary "class" text in comments or macros triggered
    # brace balancing before the original test marker was even considered.
    for m in re.finditer(r"^[ \t]*class\s+(\w+)\s*(?::[^{;]+)?\{", src, re.MULTILINE):
        cls = m.group(1)
        begin = src.find("{", m.start(), m.end())
        marker = re.search(r"\bBEGIN_TEST_CLASS\s*\(\s*"+re.escape(cls)+r"\s*\)",
                           src[begin+1:begin+2001])
        if not marker:
            continue
        try:
            end = ScopeEnd(src, begin)
        except ValueError as error:
            line = src.count("\n", 0, begin) + 1
            raise ValueError(f"{srcfile}:{line}: class {cls}: {error}") from error
        body = src[begin+1:end]
        methods = re.findall(r"(?m)^[ \t]*TEST_METHOD[ \t]*\([ \t\r\n]*(\w+)[ \t\r\n]*\)", body)
        if srcfile.name == "LongVectors.cpp":
            methods += [op+"_"+dtype for op,dtype in re.findall(
                r"\bHLK_(?:WAVEOP_)?TEST\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)", body)]
        newtests = []
        for name in dict.fromkeys(methods):
            if re.search(r"TEST_F\s*\(\s*"+cls+r"\s*,\s*"+name+r"\s*\)", src):
                continue
            newtests.append("TEST_F("+cls+", "+name+") { this->"+name+"(); }")
            registrations += 1
        setups = re.findall(r"\bTEST_(?:CLASS|METHOD)_SETUP\s*\(\s*(\w+)\s*\)", body)
        cleanups = re.findall(r"\bTEST_CLASS_CLEANUP\s*\(\s*(\w+)\s*\)", body)
        inside = ""
        if srcfile.parent.name == "HLSLExec" and (setups or cleanups):
            inside += "\nprotected:\n"
            if setups:
                inside += " void SetUp() override {\n"
                inside += "\n".join("  ASSERT_TRUE("+x+"());" for x in dict.fromkeys(setups))
                inside += "\n }\n"
            if cleanups:
                inside += " void TearDown() override {\n"
                inside += "\n".join("  EXPECT_TRUE("+x+"());" for x in dict.fromkeys(cleanups))
                inside += "\n }\n"
        patches.append((end, inside, newtests))
    for end,inside,newtests in sorted(patches, reverse=True):
        src = src[:end]+inside+src[end:]
        if newtests:
            p = src.find(";", end+len(inside))
            if p < 0: raise ValueError("Missing class terminator")
            src = src[:p+1]+"\n"+"\n".join(newtests)+"\n"+src[p+1:]
    def Method(m):
        suffix = src[m.end():].lstrip()
        return "void "+m.group(1)+"()" + ("" if suffix.startswith("{") else ";")
    src = re.sub(r"\bTEST_METHOD\s*\(\s*(\w+)\s*\)", Method, src)
    if srcfile.parent.name == "HLSLExec":
        for kind in ("TEST_CLASS_SETUP", "TEST_METHOD_SETUP", "TEST_CLASS_CLEANUP"):
            def Setup(m):
                suffix = src[m.end():].lstrip()
                return "bool "+m.group(1)+"()" + ("" if suffix.startswith("{") else ";")
            src = re.sub(r"\b"+kind+r"\s*\(\s*(\w+)\s*\)", Setup, src)
    Save(srcfile, src)
    return registrations

def Wide(value):
    out = ['L"']
    for ch in value:
        n = ord(ch)
        if ch == '"': out.append('\\"')
        elif ch == "\\": out.append("\\\\")
        elif ch == "\n": out.append("\\n")
        elif ch == "\r": out.append("\\r")
        elif ch == "\t": out.append("\\t")
        elif n<32 or n>=127: out.append("\\u%04x"%n if n<=65535 else "\\U%08x"%n)
        else: out.append(ch)
    return "".join(out)+'"'

def Data(root, generated):
    folder = root/"tools/clang/unittests/HLSLExec"
    tables = {}
    values = []
    for filename in sorted(folder.glob("*.xml")):
        doc = ET.parse(filename)
        for table in doc.findall(".//Table"):
            ident = table.get("Id")
            if not ident: raise ValueError("Table Id missing")
            key = filename.name+"#"+ident
            if key in tables: raise ValueError("Duplicate table "+key)
            rows = []
            for row in table.findall("Row"):
                name = row.get("Name")
                if name is None: raise ValueError("Row Name missing")
                rows.append(name)
                for param in row.findall("Parameter"):
                    field = param.get("Name")
                    if not field: raise ValueError("Parameter Name missing")
                    vs = param.findall("Value")
                    for value in (vs if vs else [param]):
                        values.append((key,name,field,"".join(value.itertext())))
            tables[key] = rows
    lines = ["// Generated from DXC original XML data; do not edit.",
             "#pragma once",
             "namespace adecc_dxc_test_data {",
             "struct Entry { wchar_t const* table; wchar_t const* row; wchar_t const* field; wchar_t const* value; };",
             "inline constexpr Entry entries[] = {"]
    lines += [" {"+", ".join(map(Wide,v))+"}," for v in values]
    lines += ["};", "}"]
    Save(generated/"PortableTestDataGenerated.h", "\n".join(lines)+"\n")
    original = (folder/"ExecutionTest.cpp").read_text(encoding="utf-8")
    methods = re.findall(r"BEGIN_TEST_METHOD\s*\(\s*(\w+)\s*\)(.*?)END_TEST_METHOD\s*\(\s*\)",
                         original, re.DOTALL)
    cm = ["# All original XML rows register as individual CTest executions."]
    exclude = []
    counter = 0
    for method, props in methods:
        ref = re.search(r"Table:([^\"\n ]+\.xml#[A-Za-z_0-9]+)", props)
        if not ref: continue
        key = ref.group(1)
        if key not in tables: raise ValueError("Missing XML table "+key)
        exclude.append("ExecutionTest."+method)
        for idx,row in enumerate(tables[key]):
            test = "DXC.HLSLExec."+method+"."+str(idx).zfill(5)
            cleanrow = row.replace("\\","/").replace('"','\\"').replace(";","\\;")
            cm.append('add_test(NAME "'+test+'" COMMAND "$<TARGET_FILE:ExecHLSLTests>" --HlslDataDir "${PROJECT_SOURCE_DIR}/tools/clang/test/HLSL" --gtest_filter=ExecutionTest.'+method+')')
            cm.append('set_tests_properties("'+test+'" PROPERTIES ENVIRONMENT "ADECC_DXC_TEST_TABLE='+key+';ADECC_DXC_TEST_ROW='+cleanrow+'" LABELS "BCC64X;DXC;original;xml" TIMEOUT 600)')
            counter += 1
    if not counter: raise ValueError("No data-driven DXC tests discovered")
    cm.insert(1, 'set(_adecc_xml_exclusions "'+':'.join(exclude)+'")')
    Save(generated/"OriginalXmlMethods.txt", "\n".join(exclude)+"\n")
    Save(generated/"OriginalXmlCTest.cmake", "\n".join(cm)+"\n")
    return counter,len(values)

def Main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--source-root",type=Path,required=True)
    ap.add_argument("--generated-root",type=Path,required=True)
    args=ap.parse_args()
    root=args.source_root
    files=sorted((root/"tools/clang/unittests/HLSL").glob("*.cpp"))
    files+=sorted((root/"tools/clang/unittests/HLSLExec").glob("*.cpp"))
    files.append(root/"projects/dxilconv/unittests/DxilConvTests.cpp")
    migrated=sum(Port(p) for p in files)
    rows,fields=Data(root,args.generated_root)
    print("DXC GoogleTest: %d original registrations, %d XML rows, %d values"%
          (migrated,rows,fields))
if __name__=="__main__":
    Main()
