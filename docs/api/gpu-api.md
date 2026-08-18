# NodeForge GPU Foundation API Specification

## Namespace: `gpu`

The `gpu` module provides low-level Vulkan 1.3 abstractions initialized with dynamic rendering (`VK_KHR_dynamic_rendering`) and synchronization2 (`VK_KHR_synchronization2`).

---

### 1. `gpu::Device`
Header: `src/gpu/Device.h`

Manages the Vulkan Instance, Physical Device selection, Logical Device creation, Queue management, and Vulkan Memory Allocator (VMA).

```cpp
class Device {
public:
    Device() = default;
    ~Device();

    bool Initialize(GLFWwindow* window); // Pass nullptr for headless/testing mode
    void Cleanup();

    VkInstance GetInstance() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkDevice GetDevice() const;
    VkSurfaceKHR GetSurface() const;
    VmaAllocator GetAllocator() const;
    VkQueue GetGraphicsQueue() const;
    uint32_t GetGraphicsQueueIndex() const;
    VkQueue GetPresentQueue() const;
    uint32_t GetPresentQueueIndex() const;
};
```

---

### 2. `gpu::Swapchain`
Header: `src/gpu/Swapchain.h`

Wraps the Vulkan swapchain presentation engine using `vk-bootstrap` and manages swapchain image views and rebuilding on window resize.

```cpp
class Swapchain {
public:
    explicit Swapchain(Device* device);
    ~Swapchain();

    bool Build();
    bool Rebuild();
    void Cleanup();

    VkSwapchainKHR GetSwapchain() const;
    VkFormat GetImageFormat() const;
    VkExtent2D GetExtent() const;
    const std::vector<VkImage>& GetImages() const;
    const std::vector<VkImageView>& GetImageViews() const;
};
```

---

### 3. `gpu::Texture2D`
Header: `src/gpu/Texture.h`

Encapsulates GPU texture allocation (via VMA), image views, samplers, and staging buffer CPU-to-GPU memory transfers.

```cpp
class Texture2D {
public:
    explicit Texture2D(Device* device);
    ~Texture2D();

    bool CreateProceduralCheckerboard();
    bool UploadData(const void* pixelData, uint32_t width, uint32_t height, VkDeviceSize imageSize);
    void Cleanup();

    VkImage GetImage() const;
    VkImageView GetImageView() const;
    VkSampler GetSampler() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
};
```

---

### 4. `gpu::FrameResources`
Header: `src/gpu/FrameResources.h`

Manages double/triple buffering in-flight synchronization structures (Command Pools, Command Buffers, In-Flight Fences, and Semaphores).

```cpp
struct FrameData {
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence inFlightFence = VK_NULL_HANDLE;
};

class FrameResources {
public:
    FrameResources(Device* device, size_t maxFramesInFlight = 2);
    ~FrameResources();

    bool Build();
    void Cleanup();

    FrameData& GetCurrentFrame();
    void AdvanceFrame();
};
```
