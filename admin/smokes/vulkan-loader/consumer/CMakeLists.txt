cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeVulkanLoader LANGUAGES CXX)
find_package(VulkanLoader CONFIG REQUIRED)
add_executable(vulkan-loader-consumer main.cpp)
set_target_properties(vulkan-loader-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(vulkan-loader-consumer PRIVATE Vulkan::Loader)
