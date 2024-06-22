#include "WannaVulkanBuffer.h"
#include "WannaVulkanDevice.h"
#include "WannaVulkanQueue.h"
#include "WannaVulkanCommandBuffer.h"

namespace WannaEngine {
    WannaVulkanBuffer::WannaVulkanBuffer(WannaVulkanDevice *device, VkBufferUsageFlags usage, size_t size, void *data) : mDevice(device), mSize(size) {
        VkBuffer stageBuffer;
        VkDeviceMemory stageMemory;
        CreateBufferInternal(mDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, &stageBuffer, &stageMemory); // 临时缓冲区 可由CPU直接访问

        void *mapping; // 映射内存
        CALL_VK(vkMapMemory(mDevice->getHandle(), stageMemory, 0, size, 0, &mapping));
        memcpy(mapping, data, size);
        vkUnmapMemory(mDevice->getHandle(), stageMemory);

        CreateBufferInternal(mDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, size, &mHandle, &mMemory); // 实际缓冲区 显卡单独区
        // 拷贝数据到实际缓冲区
        CopyToBuffer(mDevice, stageBuffer, mHandle, size);

        VK_DESTROY(Buffer, mDevice->getHandle(), stageBuffer);
        VK_FREE(mDevice->getHandle(), stageMemory);
    }

    WannaVulkanBuffer::~WannaVulkanBuffer() {
        VK_DESTROY(Buffer, mDevice->getHandle(), mHandle);
        VK_FREE(mDevice->getHandle(), mMemory);
    }

    void WannaVulkanBuffer::CreateBufferInternal(WannaVulkanDevice *device, VkMemoryPropertyFlags memProps, VkBufferUsageFlags usage, size_t size, VkBuffer *outBuffer, VkDeviceMemory *outMemory) {
        // 缓冲区创建信息
        VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr
        };
        CALL_VK(vkCreateBuffer(device->getHandle(), &bufferInfo, nullptr, outBuffer));

        // 获取内存信息
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(device->getHandle(), *outBuffer, &memReqs);

        // 申请缓冲区内存
        VkMemoryAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memReqs.size,
            .memoryTypeIndex = static_cast<uint32_t>(device->getMemroyIndex(memProps, memReqs.memoryTypeBits))
        };
        CALL_VK(vkAllocateMemory(device->getHandle(), &allocateInfo, nullptr, outMemory));
        CALL_VK(vkBindBufferMemory(device->getHandle(), *outBuffer, *outMemory, 0)); // 绑定内存
    }

    void WannaVulkanBuffer::CopyToBuffer(WannaVulkanDevice *device, VkBuffer srcBuffer, VkBuffer dstBuffer, size_t size) {
        VkCommandBuffer commandBuffer = device->beginDefaultCommandBuffer();
        VkBufferCopy bufferCopy = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size
        };
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
        device->submitOneCommandBuffer(commandBuffer);
    }
}