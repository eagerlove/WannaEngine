#ifndef WANNA_VULKAN_BUFFER_H
#define WANNA_VULKAN_BUFFER_H

#include "WannaVulkanCommon.h"

namespace WannaEngine {
    class WannaVulkanDevice;
    class WannaVulkanBuffer {
        public:
            WannaVulkanBuffer(WannaVulkanDevice *device, VkBufferUsageFlags usage, size_t size, void *data);
            ~WannaVulkanBuffer();

            static void CreateBufferInternal(WannaVulkanDevice *device, VkMemoryPropertyFlags memProps,VkBufferUsageFlags usage, size_t size, VkBuffer *outBuffer, VkDeviceMemory *outMemory);
            static void CopyToBuffer(WannaVulkanDevice *device, VkBuffer srcBuffer, VkBuffer dstBuffer, size_t size);

            VkBuffer getHandle() const { return mHandle; }
        private:
            VkBuffer mHandle = VK_NULL_HANDLE;
            VkDeviceMemory mMemory = VK_NULL_HANDLE;

            WannaVulkanDevice *mDevice;
            size_t mSize;
    };
}


#endif