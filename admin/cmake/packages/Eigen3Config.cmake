# BuildEngine's installed header-only Eigen 5.0.1 package config.
# Export the actual installed include root, never a build-tree path.
get_filename_component(_eigen3_package_root "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(Eigen3_VERSION "5.0.1")
set(EIGEN3_VERSION "5.0.1")
set(EIGEN3_INCLUDE_DIR "${_eigen3_package_root}/include/eigen3")
set(EIGEN3_INCLUDE_DIRS "${EIGEN3_INCLUDE_DIR}")
set(Eigen3_INCLUDE_DIRS "${EIGEN3_INCLUDE_DIR}")
if(NOT EXISTS "${EIGEN3_INCLUDE_DIR}/Eigen/Core")
   message(FATAL_ERROR "Managed Eigen headers not found: ${EIGEN3_INCLUDE_DIR}")
endif()
if(NOT TARGET Eigen3::Eigen)
   add_library(Eigen3::Eigen INTERFACE IMPORTED)
   set_target_properties(Eigen3::Eigen PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${EIGEN3_INCLUDE_DIR}")
endif()
set(Eigen3_FOUND TRUE)
