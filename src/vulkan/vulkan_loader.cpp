#include "vulkan_loader.h"

namespace dxvk::vk {

#if defined(__WINE__)

  extern "C"
  PFN_vkVoidFunction native_vkGetInstanceProcAddrWINE(VkInstance instance, const char *name);

  static PFN_vkGetInstanceProcAddr loadGetInstanceProcAddr() {
    return native_vkGetInstanceProcAddrWINE;
  }

#elif defined(__WIN32__)

  static PFN_vkGetInstanceProcAddr loadGetInstanceProcAddr() {
    static PFN_vkGetInstanceProcAddr proc = nullptr;

    if (proc)
      return proc;

    HMODULE module = ::GetModuleHandle("winevulkan.dll");

    if (!module)
      module = ::LoadLibrary("winevulkan.dll");

    if (module) {
      proc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        GetProcAddress(module, "wine_vkGetInstanceProcAddr"));
    }

    if (!proc)
      proc = vkGetInstanceProcAddr;

    return proc;
  }

#else

  static PFN_vkGetInstanceProcAddr loadGetInstanceProcAddr() {
    return vkGetInstanceProcAddr;
  }

#endif

  PFN_vkVoidFunction LibraryLoader::sym(const char* name) const {
    return loadGetInstanceProcAddr()(nullptr, name);
  }
  
  
  InstanceLoader::InstanceLoader(bool owned, VkInstance instance)
  : m_instance(instance), m_owned(owned) { }
  
  
  PFN_vkVoidFunction InstanceLoader::sym(const char* name) const {
    return loadGetInstanceProcAddr()(m_instance, name);
  }
  
  
  DeviceLoader::DeviceLoader(bool owned, VkInstance instance, VkDevice device)
  : m_getDeviceProcAddr(reinterpret_cast<PFN_vkGetDeviceProcAddr>(
      loadGetInstanceProcAddr()(instance, "vkGetDeviceProcAddr"))),
    m_device(device), m_owned(owned) { }
  
  
  PFN_vkVoidFunction DeviceLoader::sym(const char* name) const {
    return m_getDeviceProcAddr(m_device, name);
  }
  
  
  LibraryFn::LibraryFn() { }
  LibraryFn::~LibraryFn() { }
  
  
  InstanceFn::InstanceFn(bool owned, VkInstance instance)
  : InstanceLoader(owned, instance) { }
  InstanceFn::~InstanceFn() {
    if (m_owned)
      this->vkDestroyInstance(m_instance, nullptr);
  }
  
  
  DeviceFn::DeviceFn(bool owned, VkInstance instance, VkDevice device)
  : DeviceLoader(owned, instance, device) { }
  DeviceFn::~DeviceFn() {
    if (m_owned)
      this->vkDestroyDevice(m_device, nullptr);
  }
  
}