// 引擎头文件
#ifndef WANNA_ENGINE_H
#define WANNA_ENGINE_H

#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <set>
#include <unordered_set>
#include <unordered_map>

// 数组大小
#define ARRAY_SIZE(r) (sizeof(r) / sizeof(r[0]))

// Windows
#ifdef WANNA_ENGINE_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR

// Macos
#elif WANNA_ENGINE_PLATFORM_MACOS
#define VK_USE_PLATFORM_MACOS_MVK

// Linux
#elif WANNA_ENGINE_PLATFORM_LINUX
#define VK_USE_PLATFORM_XCB_KHR

#else 
    #error Unkown Platform
#endif

#define WANNA_ENGINE_GRAPHIC_API_VULKAN

#endif