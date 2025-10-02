//
// Created by User on 10/2/2025.
//

#ifndef LEARNVULKANRAII_LAYER_H
#define LEARNVULKANRAII_LAYER_H

#include "utils.h"

namespace LearnVulkanRAII
{
    class Layer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Layer)

    public:
        Layer() = default;
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}

        virtual void onUpdate() {}
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_LAYER_H