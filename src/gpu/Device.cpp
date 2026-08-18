#include "Device.h"
#include <spdlog/spdlog.h>
#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace gpu {

Device::~Device() {
    Cleanup();
}

bool Device::Initialize(GLFWwindow* window) {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("NodeForge")
                        .request_validation_layers(true)
                        .require_api_version(1, 3, 0)
                        .use_default_debug_messenger()
                        .build();

    if (!inst_ret) {
        spdlog::error("Failed to create Vulkan instance: {}", inst_ret.error().message());
        return false;
    }

    vkb::Instance vkb_inst = inst_ret.value();
    m_instance = vkb_inst.instance;
    m_debugMessenger = vkb_inst.debug_messenger;

    // Headless test might pass null window
    if (window) {
        if (glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface) != VK_SUCCESS) {
            spdlog::error("Failed to create window surface");
            return false;
        }
    }

    // Select Physical Device
    vkb::PhysicalDeviceSelector selector{vkb_inst};

    // Require Vulkan 1.3 features (Dynamic Rendering)
    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;

    selector.set_minimum_version(1, 3)
            .set_required_features_13(features13);

    if (m_surface) {
        selector.set_surface(m_surface)
                .add_required_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    } else {
        selector.require_present(false);
    }

    auto phys_ret = selector.select();
    if (!phys_ret) {
        spdlog::error("Failed to select Vulkan Physical Device: {}", phys_ret.error().message());
        return false;
    }
    vkb::PhysicalDevice vkb_phys = phys_ret.value();
    m_physicalDevice = vkb_phys.physical_device;

    // Create Logical Device
    vkb::DeviceBuilder device_builder{vkb_phys};
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        spdlog::error("Failed to create Vulkan Logical Device: {}", dev_ret.error().message());
        return false;
    }
    vkb::Device vkb_device = dev_ret.value();
    m_device = vkb_device.device;

    // Get queues
    auto graphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
    if (graphics_queue_ret) {
        m_graphicsQueue = graphics_queue_ret.value();
        m_graphicsQueueIndex = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    } else {
        spdlog::error("Failed to get graphics queue: {}", graphics_queue_ret.error().message());
        return false;
    }

    if (m_surface) {
        auto present_queue_ret = vkb_device.get_queue(vkb::QueueType::present);
        if (present_queue_ret) {
            m_presentQueue = present_queue_ret.value();
            m_presentQueueIndex = vkb_device.get_queue_index(vkb::QueueType::present).value();
        } else {
            spdlog::error("Failed to get present queue");
            return false;
        }
    }

    // Initialize VMA
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = m_physicalDevice;
    allocatorInfo.device = m_device;
    allocatorInfo.instance = m_instance;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;

    if (vmaCreateAllocator(&allocatorInfo, &m_allocator) != VK_SUCCESS) {
        spdlog::error("Failed to create VMA allocator");
        return false;
    }

    spdlog::info("Vulkan Device and VMA initialized successfully.");
    return true;
}

void Device::Cleanup() {
    if (m_allocator) {
        vmaDestroyAllocator(m_allocator);
        m_allocator = VK_NULL_HANDLE;
    }

    if (m_device) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }

    if (m_surface) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }

    if (m_debugMessenger) {
        vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
        m_debugMessenger = VK_NULL_HANDLE;
    }

    if (m_instance) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

} // namespace gpu
