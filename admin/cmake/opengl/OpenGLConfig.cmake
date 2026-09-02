get_filename_component(_OPENGL_LIBROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
get_filename_component(_OPENGL_CONFIGURATION "${_OPENGL_LIBROOT}" NAME)

# The same config is installed in two layouts:
#   package:   <root>/lib/win64/<Configuration>/cmake/OpenGL
#   published: <root>/lib/cmake/OpenGL
# The import library exists next to the CMake tree in both layouts, so it
# cannot distinguish them. Select the layout by the corresponding Mesa
# runtime pair instead.
get_filename_component(_OPENGL_PACKAGE_ROOT
  "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
set(_OPENGL_PACKAGE_BINDIR
  "${_OPENGL_PACKAGE_ROOT}/bin/win64/${_OPENGL_CONFIGURATION}")

get_filename_component(_OPENGL_PUBLISHED_ROOT
  "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(_OPENGL_PUBLISHED_BINDIR "${_OPENGL_PUBLISHED_ROOT}/bin")

if(EXISTS "${_OPENGL_PACKAGE_BINDIR}/opengl32.dll"
   AND EXISTS "${_OPENGL_PACKAGE_BINDIR}/libgallium_wgl.dll")
  set(_OPENGL_ROOT "${_OPENGL_PACKAGE_ROOT}")
  set(_OPENGL_BINDIR "${_OPENGL_PACKAGE_BINDIR}")
elseif(EXISTS "${_OPENGL_PUBLISHED_BINDIR}/opengl32.dll"
       AND EXISTS "${_OPENGL_PUBLISHED_BINDIR}/libgallium_wgl.dll")
  set(_OPENGL_ROOT "${_OPENGL_PUBLISHED_ROOT}")
  set(_OPENGL_BINDIR "${_OPENGL_PUBLISHED_BINDIR}")
else()
  message(FATAL_ERROR
    "Mesa OpenGL runtime layout is incomplete; checked "
    "${_OPENGL_PACKAGE_BINDIR} and ${_OPENGL_PUBLISHED_BINDIR}")
endif()

if(NOT EXISTS "${_OPENGL_LIBROOT}/opengl32.lib")
  message(FATAL_ERROR
    "Mesa OpenGL import library not found: ${_OPENGL_LIBROOT}/opengl32.lib")
endif()

if(NOT TARGET OpenGL::OpenGL)
  add_library(OpenGL::OpenGL INTERFACE IMPORTED)
  set_target_properties(OpenGL::OpenGL PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_OPENGL_ROOT}/include"
    INTERFACE_LINK_LIBRARIES "${_OPENGL_LIBROOT}/opengl32.lib")
endif()

set(OpenGL_RUNTIME_DLL "${_OPENGL_BINDIR}/opengl32.dll")
set(OpenGL_GALLIUM_DLL "${_OPENGL_BINDIR}/libgallium_wgl.dll")
set(OpenGL_FOUND TRUE)

unset(_OPENGL_BINDIR)
unset(_OPENGL_CONFIGURATION)
unset(_OPENGL_LIBROOT)
unset(_OPENGL_PACKAGE_BINDIR)
unset(_OPENGL_PACKAGE_ROOT)
unset(_OPENGL_PUBLISHED_BINDIR)
unset(_OPENGL_PUBLISHED_ROOT)
unset(_OPENGL_ROOT)
