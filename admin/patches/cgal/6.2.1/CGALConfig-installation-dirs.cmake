# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
#
# CGAL 6.2.1's installed CGALConfig.cmake includes this optional companion.
# The BuildEngine install layout is deeper than the default lib/cmake/CGAL:
#    <package-root>/lib/win64/<Configuration>/cmake/CGAL
#
# CGALConfig.cmake otherwise walks up at most four directory levels and
# incorrectly concludes that <package-root>/lib is the package root.
# Resolve relative to this config file; no build-host or absolute paths.
get_filename_component(CGAL_ROOT
   "${CGAL_CONFIG_DIR}/../../../../.." ABSOLUTE)
if(NOT EXISTS "${CGAL_ROOT}/include/CGAL/config.h")
   message(FATAL_ERROR
      "CGAL 6.2.1 installed root is invalid: ${CGAL_ROOT}; "
      "expected include/CGAL/config.h alongside installed package")
endif()
