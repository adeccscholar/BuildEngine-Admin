# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# CGAL 6.2.1: choose the exact package root from this installed config path.
# Supported layouts:
#   <root>/lib/win64/<Release|Debug>/cmake/CGAL
#   <root>/lib/cmake/CGAL
# Never resolve CGAL from PATH or a different installation.
set(_adecc_cgal_config_dir "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(_adecc_cgal_root_variant
   "${_adecc_cgal_config_dir}/../../../../.." ABSOLUTE)
get_filename_component(_adecc_cgal_root_generic
   "${_adecc_cgal_config_dir}/../../.." ABSOLUTE)
if(EXISTS "${_adecc_cgal_root_variant}/include/CGAL/config.h")
   set(CGAL_ROOT "${_adecc_cgal_root_variant}")
elseif(EXISTS "${_adecc_cgal_root_generic}/include/CGAL/config.h")
   set(CGAL_ROOT "${_adecc_cgal_root_generic}")
else()
   message(FATAL_ERROR
      "CGAL 6.2.1 installed package cannot resolve include/CGAL/config.h. "
      "ConfigDir=${_adecc_cgal_config_dir}; "
      "VariantRoot=${_adecc_cgal_root_variant}; "
      "GenericRoot=${_adecc_cgal_root_generic}")
endif()
message(STATUS "[CGAL] Exact installed package root: ${CGAL_ROOT}")
