cmake_minimum_required(VERSION 3.22)

add_library(wanna_core
    Private/WannaApplication.cpp
)
target_link_libraries(wanna_core PUBLIC wanna_platform)