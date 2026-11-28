# Version-pinned BuildEngine Vulkan-Headers 1.4.368 interface.
# This config exists only in the isolated Vulkan-Loader source workspace.
if(NOT DEFINED VULKAN_HEADERS_INSTALL_DIR OR
   NOT EXISTS "${VULKAN_HEADERS_INSTALL_DIR}/include/vulkan/vulkan.h")
   message(FATAL_ERROR
      "Vulkan-Loader requires the active BuildEngine vulkan-headers package")
endif()
if(NOT TARGET Vulkan::Headers)
   add_library(Vulkan::Headers INTERFACE IMPORTED)
   set_target_properties(Vulkan::Headers PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_HEADERS_INSTALL_DIR}/include")
endif()
