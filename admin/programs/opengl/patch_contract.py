#!/usr/bin/env python3
from __future__ import annotations

import re
from pathlib import Path

contract = Path("admin/build-libraries.xml")
text = contract.read_text(encoding="utf-8")

mesa_block = r'''<library id="opengl" version="26.2.1" timestamp="2026-09-01T23:21:00Z">
      <metadata name="Mesa OpenGL" supplier="Mesa project" homepage="https://mesa3d.org/">
         <license name="Mesa component licenses" file="docs\license.rst"/>
      </metadata>
      <source>
         <!-- Official Mesa release archive; checksum is published in the Mesa 26.2.1 release notes. -->
         <download url="https://archive.mesa3d.org/mesa-{LibraryVersion}.tar.xz" archive="mesa-{LibraryVersion}.tar.xz" sha256="c47e81bddc4760360a41ac3c5acec38acb81f9d750ecef47e7f3adc7021a4442"/>
         <extract format="libarchive" root="mesa-{LibraryVersion}">
            <require path="meson.build"/>
            <require path="meson.options"/>
            <require path="VERSION"/>
            <require path="docs\license.rst"/>
            <require path="include\GL\gl.h"/>
            <require path="include\GL\glext.h"/>
            <require path="include\GL\glcorearb.h"/>
            <require path="include\GL\wglext.h"/>
            <require path="src\gallium\targets\wgl\meson.build"/>
            <require path="src\gallium\targets\libgl-gdi\meson.build"/>
         </extract>
         <!-- Meson is part of the reproducibility contract, not a host prerequisite. -->
         <download url="https://files.pythonhosted.org/packages/source/m/meson/meson-1.12.0.tar.gz" archive="meson-1.12.0.tar.gz" sha256="88afe0c20e52030218924ac37d0c81c59b4b5f3ae3752c8c6d7470c7d365886c"/>
         <extract format="libarchive" root="meson-1.12.0">
            <require path="meson.py"/>
            <require path="mesonbuild\mesonmain.py"/>
            <require path="COPYING"/>
         </extract>
         <copy source="{BuildFileDir}\programs\opengl\meson_bootstrap.py" target="{Workspace}\.buildengine\meson_bootstrap.py" overwrite="true" preserveCurrentArtifact="true"/>
      </source>
      <build testsAffectBuild="true">
         <!-- Native BCC64X Mesa build. No MSVC, clang-cl or MinGW fallback is permitted. -->
         <environment name="CC" value="{Tool:bcc64x}"/>
         <environment name="CXX" value="{Tool:bcc64x}"/>
         <environment name="AR" value="{Tool:llvm-ar}"/>
         <environment name="WINDRES" value="{Tool:bcc64x-windres}"/>
         <environment name="PATH" value="{ToolDir:bcc64x};{ToolDir:llvm-ar};{ToolDir:bcc64x-windres};{ToolDir:ninja};{ToolDir:python};{ENV:PATH}"/>

         <execute name="remove-build" executable="{Tool:cmake}" workingDirectory="{ProductionRoot}" showOutput="false">
            <argument value="-E"/><argument value="remove_directory"/><argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}"/>
         </execute>
         <target name="meson-setup" type="meson" executable="{Tool:python}" workingDirectory="{ProductionRoot}" showOutput="false">
            <argument value="{Workspace}\.buildengine\meson_bootstrap.py"/>
            <argument value="{Workspace}\meson-1.12.0"/>
            <argument value="setup"/>
            <argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}"/>
            <argument value="{Workspace}\mesa-{LibraryVersion}"/>
            <argument value="--backend=ninja"/>
            <argument value="--prefix={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}"/>
            <argument value="--bindir=bin\win64\{Configuration}"/>
            <argument value="--libdir=lib\win64\{Configuration}"/>
            <argument value="--includedir=include"/>
            <argument value="--buildtype={MesonBuildType}"/>
            <argument value="-Dplatforms=windows"/>
            <argument value="-Dgallium-drivers=softpipe"/>
            <argument value="-Dgallium-wgl-dll-name=libgallium_wgl"/>
            <argument value="-Dopengl=true"/>
            <argument value="-Dgles1=disabled"/>
            <argument value="-Dgles2=disabled"/>
            <argument value="-Degl=disabled"/>
            <argument value="-Dglx=disabled"/>
            <argument value="-Dgbm=disabled"/>
            <argument value="-Dvulkan-drivers="/>
            <argument value="-Dvulkan-layers="/>
            <argument value="-Dllvm=disabled"/>
            <argument value="-Dzlib=disabled"/>
            <argument value="-Dzstd=disabled"/>
            <argument value="-Dxmlconfig=disabled"/>
            <argument value="-Dbuild-tests=true"/>
            <argument value="-Dtools="/>
            <argument value="-Dgallium-rusticl=false"/>
         </target>
         <target name="meson-compile" type="meson" executable="{Tool:python}" workingDirectory="{ProductionRoot}" showOutput="false">
            <argument value="{Workspace}\.buildengine\meson_bootstrap.py"/>
            <argument value="{Workspace}\meson-1.12.0"/>
            <argument value="compile"/>
            <argument value="-C"/>
            <argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}"/>
         </target>
         <target name="meson-test" type="meson" executable="{Tool:python}" workingDirectory="{ProductionRoot}" showOutput="false" test="true">
            <argument value="{Workspace}\.buildengine\meson_bootstrap.py"/>
            <argument value="{Workspace}\meson-1.12.0"/>
            <argument value="test"/>
            <argument value="-C"/>
            <argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}"/>
            <argument value="--print-errorlogs"/>
         </target>
         <variant name="Release">
            <variable name="MesonBuildType" value="release"/>
         </variant>
         <variant name="Debug">
            <variable name="MesonBuildType" value="debug"/>
         </variant>
         <install>
            <perVariant>
               <target name="meson-install" type="meson" executable="{Tool:python}" workingDirectory="{ProductionRoot}" showOutput="false">
                  <argument value="{Workspace}\.buildengine\meson_bootstrap.py"/>
                  <argument value="{Workspace}\meson-1.12.0"/>
                  <argument value="install"/>
                  <argument value="-C"/>
                  <argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}"/>
                  <argument value="--no-rebuild"/>
               </target>
               <copy source="{BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}\bin\win64\{Configuration}" target="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}" recursive="true" overwrite="true"/>
               <execute name="create-libdir" executable="{Tool:cmake}" workingDirectory="{ProductionRoot}" showOutput="false">
                  <argument value="-E"/><argument value="make_directory"/><argument value="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}"/>
               </execute>
               <!-- Import libraries are derived only from the BCC64X-built Mesa DLLs. -->
               <execute name="opengl-def" executable="{Tool:tdump}" workingDirectory="{ProductionRoot}" showOutput="false">
                  <argument value="-def"/><argument value="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\opengl32.dll"/><argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\opengl32.def"/>
               </execute>
               <execute name="opengl-import" executable="{Tool:ld-lld}" workingDirectory="{ProductionRoot}" showOutput="false">
                  <argument value="-m"/><argument value="i386pep"/><argument value="--out-implib"/><argument value="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\opengl32.lib"/><argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\opengl32.def"/>
               </execute>
               <execute name="wgl-def" executable="{Tool:tdump}" workingDirectory="{ProductionRoot}" showOutput="false">
                  <argument value="-def"/><argument value="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\libgallium_wgl.dll"/><argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\libgallium_wgl.def"/>
               </execute>
               <execute name="wgl-import" executable="{Tool:ld-lld}" workingDirectory="{ProductionRoot}" showOutput="false">
                  <argument value="-m"/><argument value="i386pep"/><argument value="--out-implib"/><argument value="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\libgallium_wgl.lib"/><argument value="{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\libgallium_wgl.def"/>
               </execute>
               <copy source="{BuildFileDir}\cmake\opengl\OpenGLConfig.cmake" target="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\cmake\OpenGL\OpenGLConfig.cmake" overwrite="true"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\opengl32.dll"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\libgallium_wgl.dll"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\opengl32.lib"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\libgallium_wgl.lib"/>
            </perVariant>
            <common>
               <copy source="{Workspace}\mesa-{LibraryVersion}\include\GL" target="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\GL" recursive="true" overwrite="true"/>
               <copy source="{Workspace}\mesa-{LibraryVersion}\docs\license.rst" target="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE-MESA.rst" overwrite="true"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\GL\gl.h"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\GL\glext.h"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\GL\glcorearb.h"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\GL\wglext.h"/>
               <require path="{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\LICENSE-MESA.rst"/>
            </common>
         </install>
      </build>
      <publish root="Win64x" configuration="Release" consumer="{BuildFileDir}\cmake\consumer">
         <tree source="include" target="include"/>
         <files source="lib\win64\{Configuration}" target="lib" extensions=".lib"/>
         <files source="bin\win64\{Configuration}" target="bin" extensions=".dll"/>
         <cmake source="lib\win64\{Configuration}\cmake" target="lib\cmake"/>
      </publish>
      <smoke id="consumer" source="smokes\opengl\consumer" configuration="Release" cmake="{Tool:cmake}" ninja="{Tool:ninja}" toolchain="{BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake" executable="opengl-consumer.exe">
         <environment name="CB_BCC64X" value="{Tool:bcc64x}"/>
         <environment name="CB_BDS" value="{BDS}"/>
         <environment name="PATH" value="{InstallRoot}\Win64x\bin;{ENV:PATH}"/>
      </smoke>
   </library>'''

pattern = re.compile(r'<library id="opengl"\b.*?</library>', re.DOTALL)
text, count = pattern.subn(mesa_block, text, count=1)
if count != 1:
    raise SystemExit(f"expected one OpenGL library block, replaced {count}")

old_refs = text.count("registry-20260828")
text = text.replace("registry-20260828", "26.2.1")
if old_refs < 2:
    raise SystemExit(f"expected at least two dependent OpenGL version references, found {old_refs}")

text = text.replace('<library id="glew" version="2.3.1" timestamp="2026-09-01T12:45:00Z">',
                    '<library id="glew" version="2.3.1" timestamp="2026-09-01T23:21:00Z">')
text = text.replace('<library id="raylib" version="6.0" timestamp="2026-09-01T14:43:00Z">',
                    '<library id="raylib" version="6.0" timestamp="2026-09-01T23:21:00Z">')

contract.write_text(text, encoding="utf-8", newline="\n")
