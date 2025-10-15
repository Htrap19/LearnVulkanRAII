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

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
include_directories(Core/src)

add_subdirectory(Core)
add_subdirectory(App)

