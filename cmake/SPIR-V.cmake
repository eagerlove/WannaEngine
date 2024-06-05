# 编译着色器文件

find_program(GLSL_COMPILE_COMMAND glslangValidator)
if (NOT GLSL_COMPILE_COMMAND)
    message(FATAL_ERROR "glslangValidator not found - source maintained at https://github.com/KhronosGroup/glslang")
endif()

function(shaders ret)
    set(options)
    set(oneValueArgs SPIRV_VERSION)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(_spirvshaders "${options}" "${oneValueArgs}"
    "${multiValueArgs}" ${ARGN})

    if(NOT _spirvshaders_SPIRV_VERSION)
    set(_spirvshaders_SPIRV_VERSION 1.0)
    endif()

    foreach(GLSL ${_spirvshaders_SOURCES})
        string(MAKE_C_IDENTIFIER ${GLSL} IDENTIFIER)
        set(HEADER "${WAN_DEFINE_RES_ROOT_DIR}Shader/${GLSL}.spv")
        set(GLSL "${WAN_DEFINE_RES_ROOT_DIR}/${GLSL}")

        message("GLSL Command: ${GLSL_COMPILE_COMMAND} -V --target-env spirv${_spirvshaders_SPIRV_VERSION} ${GLSL} -o ${HEADER}")

        add_custom_command(
                OUTPUT ${HEADER}
                COMMAND ${GLSL_COMPILE_COMMAND} -V --target-env spirv${_spirvshaders_SPIRV_VERSION} ${GLSL} -o ${HEADER}
                DEPENDS ${GLSL})
        list(APPEND HEADERS ${HEADER})
    endforeach()

    set(${ret} "${HEADERS}" PARENT_SCOPE)

endfunction(shaders)
