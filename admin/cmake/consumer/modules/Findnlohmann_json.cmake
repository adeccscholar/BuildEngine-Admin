# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT

include(FindPackageHandleStandardArgs)

find_path(nlohmann_json_INCLUDE_DIR NAMES nlohmann/json.hpp)
find_package_handle_standard_args(nlohmann_json REQUIRED_VARS nlohmann_json_INCLUDE_DIR)

if(nlohmann_json_FOUND AND NOT TARGET nlohmann_json::nlohmann_json)
   add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
   set_target_properties(nlohmann_json::nlohmann_json PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${nlohmann_json_INCLUDE_DIR}")
endif()

mark_as_advanced(nlohmann_json_INCLUDE_DIR)
