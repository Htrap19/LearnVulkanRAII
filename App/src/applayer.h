//
// Created by User on 10/3/2025.
//

#ifndef LEARNVULKANRAII_APPLAYER_H
#define LEARNVULKANRAII_APPLAYER_H

#include "layer.h"

using namespace LearnVulkanRAII;

class AppLayer : public Layer
{
public:
    DEFINE_SMART_POINTER_HELPERS(AppLayer);

public:
    AppLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;

    virtual void onUpdate() override;
};

#endif //LEARNVULKANRAII_APPLAYER_H