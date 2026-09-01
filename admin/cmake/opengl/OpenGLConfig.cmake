get_filename_component(_OPENGL_LIBROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
get_filename_component(_OPENGL_PKGROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
if(NOT TARGET OpenGL::OpenGL)
  add_library(OpenGL::OpenGL INTERFACE IMPORTED)
  set_target_properties(OpenGL::OpenGL PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_OPENGL_PKGROOT}/include"
    INTERFACE_LINK_LIBRARIES "${_OPENGL_LIBROOT}/opengl32.lib")
endif()
set(OpenGL_FOUND TRUE)
unset(_OPENGL_LIBROOT)
unset(_OPENGL_PKGROOT)
