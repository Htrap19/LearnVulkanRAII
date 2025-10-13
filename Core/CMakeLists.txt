find_package(Vulkan REQUIRED)

CPMAddPackage(
    NAME glslang
    GITHUB_REPOSITORY KhronosGroup/glslang
    GIT_TAG vulkan-sdk-1.4.309
    OPTIONS
        "ENABLE_OPT OFF"
        "ENABLE_HLSL OFF"
)

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
        # base
        src/base/window.cpp
        src/base/window.h
        src/base/utils.h
        src/base/application.cpp
        src/base/application.h
        src/base/graphicscontext.cpp
        src/base/graphicscontext.h
        src/base/layer.h
        src/base/layerstack.cpp
        src/base/layerstack.h

        # event
        src/event/event.h
        src/event/windowevent.h
        src/event/keyevent.h
        src/base/inputcodes.h
        src/event/mouseevent.h
        src/mesh/mesh.h
        src/renderer/renderer.cpp
        src/renderer/renderer.h
        src/renderer/utils/utils.cpp
        src/renderer/utils/utils.h
        src/renderer/framebuffer.cpp
        src/renderer/framebuffer.h
)

target_link_libraries(LearnVulkanRAIICore PUBLIC
        Vulkan::Vulkan
        Vulkan::Hpp
        glfw
        glslang::glslang
        glslang::glslang-default-resource-limits)