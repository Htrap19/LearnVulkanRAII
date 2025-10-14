add_executable(LearnVulkanRAII src/app.h
        src/applayer.cpp
        src/applayer.h)

target_link_libraries(LearnVulkanRAII PUBLIC LearnVulkanRAIICore)