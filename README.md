# 基于Vulkan制作的跨平台渲染引擎

### 外部引用库(/Platform/extern)
- spdlog 日志系统 查看系统状态
- glfw   图形库 建立窗口
### 图形API配置
- Vulkan
    - 层 特征支持
    - 拓展 图形优化
    - Vulkan实例
    - 表面
    - 物理设备与逻辑设备
    - 命令队列
    - 交换链
    - 渲染管线

Vulkan渲染基本流程：
- 查询当前环境下支持的层和拓展
- 再针对这些层和拓展创建Vulkan实例(Instance)
- 针对不同系统平台选取窗口表面(Surface)
- 之后选取物理设备建立逻辑设备
- 通过命令队列将任务传递给逻辑设备
- 设置缓冲区 编译着色器 通过交换链在窗口Surface上渲染图形

