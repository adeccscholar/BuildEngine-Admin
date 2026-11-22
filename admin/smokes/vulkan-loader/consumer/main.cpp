#include <vulkan/vulkan.h>
#include <print>
#include <cstdint>
int main() {
   std::uint32_t uVersion {};
   VkResult const eResult=vkEnumerateInstanceVersion(&uVersion);
   bool const bOK=eResult==VK_SUCCESS && VK_API_VERSION_MAJOR(uVersion)>=1;
   std::println("SMOKE|CHECK|vulkan-loader-version|{}|Vulkan API version {}.{}.{}",
      bOK?"PASS":"FAIL",VK_API_VERSION_MAJOR(uVersion),
      VK_API_VERSION_MINOR(uVersion),VK_API_VERSION_PATCH(uVersion));
   std::println("SMOKE|RESULT|{}|Managed Vulkan Loader BCC64X",
      bOK?"PASS":"FAIL");
   return bOK?0:1;
}
