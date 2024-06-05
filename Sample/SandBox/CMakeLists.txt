cmake_minimum_required(VERSION 3.22)

# 着色器文件配置
set(SHADERS 
    shader.frag 
    shader.vert
)
shaders(
    SHADER_HEADERS
    SPIRV_VERSION
    1.0
    SOURCES
    ${SHADERS}
)

add_executable(SandBox ${SHADER_HEADERS} Main.cpp)
target_link_libraries(SandBox PRIVATE wanna_core)