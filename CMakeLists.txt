cmake_minimum_required(VERSION 4.0)
project(LearnVulkanRAII)

set(CMAKE_CXX_STANDARD 23)

set(CPM_DOWNLOAD_VERSION 0.42.0)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

find_package(Vulkan REQUIRED)

CPMAddPackage(
    NAME Vulkan-Hpp
    GITHUB_REPOSITORY KhronosGroup/Vulkan-Hpp
    GIT_TAG v1.4.309
)

CPMAddPackage(
    NAME glfw
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.4
)

add_executable(LearnVulkanRAII main.cpp
        window.cpp
        window.h
        utils.h
        application.cpp
        application.h
        graphicscontext.cpp
        graphicscontext.h
        layer.h
        layerstack.cpp
        layerstack.h
)
target_link_libraries(LearnVulkanRAII PRIVATE Vulkan::Vulkan Vulkan::Hpp glfw)
