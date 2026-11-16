cmake_minimum_required(VERSION 3.31)
project(adobe_xmp_toolkit LANGUAGES C CXX)

# Adobe XMP Toolkit carried by the pinned DNG SDK 1.7.1 Build 2802 source.
# Build the full XMPCore and XMPFiles shared-library pair with the managed
# Expat and zlib packages, not bundled copies or an alternative toolchain.
include(GNUInstallDirs)
set(theXmpRoot "${CMAKE_CURRENT_SOURCE_DIR}/xmp/toolkit")
foreach(theRoot XMP_EXPAT_ROOT XMP_ZLIB_ROOT)
   if(NOT DEFINED ${theRoot} OR NOT IS_DIRECTORY "${${theRoot}}")
      message(FATAL_ERROR "Managed XMP dependency root missing: ${theRoot}")
   endif()
endforeach()
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
   message(FATAL_ERROR "Unsupported XMP configuration: ${CMAKE_BUILD_TYPE}")
endif()
find_library(theExpatLibrary NAMES libexpat libexpatd expat expatd
   PATHS "${XMP_EXPAT_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}"
   NO_DEFAULT_PATH REQUIRED)
find_library(theZlibLibrary NAMES libz libzd z zlib
   PATHS "${XMP_ZLIB_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}"
   NO_DEFAULT_PATH REQUIRED)

# Source groups follow the pinned Adobe XMPCore/XMPFiles CMake producers.
# Use only their Windows implementation units; no Darwin/Linux/Android units,
# no bundled Expat or zlib sources and no private C runtime.
file(GLOB_RECURSE theCommonSources CONFIGURE_DEPENDS
   "${theXmpRoot}/XMPCommon/source/*.cpp"
   "${theXmpRoot}/public/include/XMPCommon/source/*.cpp")
file(GLOB_RECURSE theCoreDomSources CONFIGURE_DEPENDS
   "${theXmpRoot}/public/include/XMPCore/source/*.cpp")
file(GLOB theCoreSources CONFIGURE_DEPENDS
   "${theXmpRoot}/XMPCore/source/*.cpp")
list(REMOVE_ITEM theCoreSources
   "${theXmpRoot}/XMPCore/source/Android_Utils.cpp")
file(GLOB theUuidSources CONFIGURE_DEPENDS
   "${theXmpRoot}/third-party/zuid/sources/*.cpp")
set(theCoreUtilities
   "${theXmpRoot}/source/UnicodeConversions.cpp"
   "${theXmpRoot}/source/XML_Node.cpp"
   "${theXmpRoot}/source/XMP_LibUtils.cpp")
add_library(XMPCore SHARED
   ${theCoreSources} ${theCommonSources} ${theCoreDomSources}
   ${theUuidSources} ${theCoreUtilities})
set_target_properties(XMPCore PROPERTIES
   PREFIX "" IMPORT_PREFIX "" IMPORT_SUFFIX ".lib"
   WINDOWS_EXPORT_ALL_SYMBOLS ON
   CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
set_property(TARGET XMPCore PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_definitions(XMPCore PRIVATE
   WIN_ENV=1 XMP_DynamicBuild=1
   BUILDING_XMPCORE_LIB=1 BUILDING_XMPCORE_AS_DYNAMIC=1
   XMP_COMPONENT_INT_NAMESPACE=AdobeXMPCore_Int
   AdobePrivate=1
)
target_include_directories(XMPCore PRIVATE
   "${theXmpRoot}" "${theXmpRoot}/public/include"
   "${theXmpRoot}/XMPCore/resource/win"
   "${XMP_EXPAT_ROOT}/include" "${XMP_ZLIB_ROOT}/include")
target_link_libraries(XMPCore PRIVATE
   "${theExpatLibrary}" "${theZlibLibrary}" rpcrt4 ole32)

file(GLOB_RECURSE theFileSources CONFIGURE_DEPENDS
   "${theXmpRoot}/XMPFiles/source/*.cpp")
# Adobe's own project explicitly compiles only the matching OS implementations.
list(FILTER theFileSources EXCLUDE REGEX
   "(Host_IO-(POSIX|Mac)|OS_Utils_(Linux|Mac|Android|ANDROID)|_Mac\\.cpp|_Linux\\.cpp|_Android\\.cpp)$")
list(FILTER theFileSources EXCLUDE REGEX
   "(Host_IO-(POSIX|Mac)|OS_Utils_(Linux|Mac|Android|ANDROID))\\.cpp$")
set(theFileUtilities
   "${theXmpRoot}/source/UnicodeConversions.cpp"
   "${theXmpRoot}/source/SafeStringAPIs.cpp"
   "${theXmpRoot}/source/PerfUtils.cpp"
   "${theXmpRoot}/source/XIO.cpp"
   "${theXmpRoot}/source/IOUtils.cpp"
   "${theXmpRoot}/source/XML_Node.cpp"
   "${theXmpRoot}/source/XMP_LibUtils.cpp"
   "${theXmpRoot}/source/XMP_ProgressTracker.cpp"
   "${theXmpRoot}/source/XMPFiles_IO.cpp"
   "${theXmpRoot}/source/Host_IO-Win.cpp"
   "${theXmpRoot}/third-party/zuid/sources/MD5.cpp")
add_library(XMPFiles SHARED ${theFileSources} ${theFileUtilities})
set_target_properties(XMPFiles PROPERTIES
   PREFIX "" IMPORT_PREFIX "" IMPORT_SUFFIX ".lib"
   WINDOWS_EXPORT_ALL_SYMBOLS ON
   CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
set_property(TARGET XMPFiles PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_definitions(XMPFiles PRIVATE
   WIN_ENV=1 XMP_DynamicBuild=1
   BUILDING_XMPFILES_LIB=1 BUILDING_XMPFILES_AS_DYNAMIC=1
   AdobePrivate=1
)
target_include_directories(XMPFiles PRIVATE
   "${theXmpRoot}" "${theXmpRoot}/public/include"
   "${theXmpRoot}/public/include/client-glue"
   "${theXmpRoot}/XMPFilesPlugins/api/source"
   "${theXmpRoot}/XMPFiles/resource/win"
   "${XMP_EXPAT_ROOT}/include" "${XMP_ZLIB_ROOT}/include")
target_link_libraries(XMPFiles PRIVATE XMPCore
   "${theExpatLibrary}" "${theZlibLibrary}" rpcrt4 ole32)

install(TARGETS XMPCore XMPFiles
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${theXmpRoot}/public/include/"
   DESTINATION include/xmp)
