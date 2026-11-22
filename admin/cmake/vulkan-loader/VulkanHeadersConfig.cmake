# BuildEngine native BCC64X adapter for the separately managed Vulkan-Headers package.
# Vulkan-Headers is header-only and has no configured target export in its
# existing BuildEngine installation. Do not fetch additional headers.
if(NOT DEFINED VULKAN_HEADERS_PACKAGE_ROOT OR
   NOT EXISTS "${VULKAN_HEADERS_PACKAGE_ROOT}/include/vulkan/vulkan.h")
   message(FATAL_ERROR "Managed Vulkan-Headers package missing")
endif()
if(NOT TARGET Vulkan::Headers)
   add_library(Vulkan::Headers INTERFACE IMPORTED)
   set_target_properties(Vulkan::Headers PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_HEADERS_PACKAGE_ROOT}/include")
endif()
set(VulkanHeaders_VERSION "1.4.368")
set(VulkanHeaders_FOUND TRUE)
