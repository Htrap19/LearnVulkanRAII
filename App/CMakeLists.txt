add_executable(LearnVulkanRAII src/app.h
        src/applayer.cpp
        src/applayer.h
        src/appwindow.cpp
        src/appwindow.h
        src/app.cpp)

target_link_libraries(LearnVulkanRAII PUBLIC LearnVulkanRAIICore)