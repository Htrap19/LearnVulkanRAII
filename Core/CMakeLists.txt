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

add_library(LearnVulkanRAIICore STATIC
        src/window.cpp
        src/window.h
        src/utils.h
        src/application.cpp
        src/application.h
        src/graphicscontext.cpp
        src/graphicscontext.h
        src/layer.h
        src/layerstack.cpp
        src/layerstack.h)

target_link_libraries(LearnVulkanRAIICore PUBLIC Vulkan::Vulkan Vulkan::Hpp glfw)