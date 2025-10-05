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
)

target_link_libraries(LearnVulkanRAIICore PUBLIC Vulkan::Vulkan Vulkan::Hpp glfw)