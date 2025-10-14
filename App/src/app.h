#ifndef LEARNVULKANRAII_APP_H
#define LEARNVULKANRAII_APP_H

#include "base/application.h"

using namespace LearnVulkanRAII;

class App final : public Application
{
public:
    DEFINE_SMART_POINTER_HELPERS(App);

public:
    App();
};

#endif //LEARNVULKANRAII_APP_H