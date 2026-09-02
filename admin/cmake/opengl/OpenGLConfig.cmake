get_filename_component(_OPENGL_LIBROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

# The same config is used in two layouts:
#   package:   <root>/lib/win64/<Configuration>/cmake/OpenGL
#   published: <root>/lib/cmake/OpenGL
# Detect the published layout by the import library next to the CMake tree.
if(EXISTS "${_OPENGL_LIBROOT}/opengl32.lib")
  get_filename_component(_OPENGL_PKGROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
  set(_OPENGL_BINDIR "${_OPENGL_PKGROOT}/bin")
else()
  get_filename_component(_OPENGL_PKGROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
  get_filename_component(_OPENGL_CONFIGURATION "${_OPENGL_LIBROOT}" NAME)
  set(_OPENGL_BINDIR "${_OPENGL_PKGROOT}/bin/win64/${_OPENGL_CONFIGURATION}")
endif()

if(NOT TARGET OpenGL::OpenGL)
  add_library(OpenGL::OpenGL INTERFACE IMPORTED)
  set_target_properties(OpenGL::OpenGL PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_OPENGL_PKGROOT}/include"
    INTERFACE_LINK_LIBRARIES "${_OPENGL_LIBROOT}/opengl32.lib")
endif()

set(OpenGL_RUNTIME_DLL "${_OPENGL_BINDIR}/opengl32.dll")
set(OpenGL_GALLIUM_DLL "${_OPENGL_BINDIR}/libgallium_wgl.dll")
set(OpenGL_FOUND TRUE)

unset(_OPENGL_BINDIR)
unset(_OPENGL_CONFIGURATION)
unset(_OPENGL_LIBROOT)
unset(_OPENGL_PKGROOT)
