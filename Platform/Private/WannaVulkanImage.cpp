#include "WannaVulkanImage.h"
#include "WannaVulkanDevice.h"

namespace WannaEngine {
    WannaVulkanImage::WannaVulkanImage(WannaVulkanDevice *device, VkExtent3D extent, VkFormat format,  VkImageUsageFlags usage)
            : mDevice(device), mFormat(format), mExtent(extent), mUsage(usage) {
        // 创建图像
        VkImageTiling tiling = VK_IMAGE_TILING_LINEAR;
        bool isDepthStencilFormat = IsDepthStencilFormat(format);
        if (isDepthStencilFormat) {
            tiling = VK_IMAGE_TILING_OPTIMAL;
        }

        VkImageCreateInfo imageInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D, // 默认2D图片
            .format = format, // 附件格式(颜色或深度)
            .extent = extent,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = tiling,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED // 图像布局 初始化为未定义
        };
        CALL_VK(vkCreateImage(mDevice->getHandle(), &imageInfo, nullptr, &mHandle));

        // 获取内存信息
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(mDevice->getHandle(), mHandle, &memReqs);

        // 申请图像内存
        VkMemoryAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memReqs.size,
            .memoryTypeIndex = static_cast<uint32_t>(mDevice->getMemroyIndex(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memReqs.memoryTypeBits))
        };
        CALL_VK(vkAllocateMemory(mDevice->getHandle(), &allocateInfo, nullptr, &mMemory));
        CALL_VK(vkBindImageMemory(mDevice->getHandle(), mHandle, mMemory, 0)); // 绑定内存和图像

    }

    WannaVulkanImage::WannaVulkanImage(WannaVulkanDevice *device, VkImage image, VkExtent3D extent, VkFormat format,  VkImageUsageFlags usage)
             : mDevice(device), mHandle(image), mFormat(format), mExtent(extent), mUsage(usage), bCreateImage(false) {

    }

    WannaVulkanImage::~WannaVulkanImage() {
        if (bCreateImage) {
            VK_DESTROY(Image, mDevice->getHandle(), mHandle);
            VK_FREE(mDevice->getHandle(), mMemory);
        }
    }
}