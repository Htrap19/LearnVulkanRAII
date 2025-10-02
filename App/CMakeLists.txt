add_executable(LearnVulkanRAII src/main.cpp
        src/applayer.cpp
        src/applayer.h)

target_link_libraries(LearnVulkanRAII PUBLIC LearnVulkanRAIICore)