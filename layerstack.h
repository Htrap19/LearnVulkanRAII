//
// Created by User on 10/3/2025.
//

#ifndef LEARNVULKANRAII_LAYERSTACK_H
#define LEARNVULKANRAII_LAYERSTACK_H

#include "layer.h"

#include <vector>

namespace LearnVulkanRAII
{
    class LayerStack
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(LayerStack)

        using LayerListType = std::vector<Layer::Shared>;

    public:
        LayerStack();
        virtual ~LayerStack();

        void pushLayer(const Layer::Shared& layer);
        void popLayer(const Layer::Shared& layer);
        void pushOverlay(const Layer::Shared& layer);
        void popOverlay(const Layer::Shared& layer);

        // TODO: Need to implement the push/pop later
        // - pushLayerLater()
        // - popLayerLater()
        // - pushOverlayLater()
        // - popOverlayLater()

        virtual void onUpdate();

    private:
        LayerListType m_layers;
        size_t m_layerInsertIndex = 0;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_LAYERSTACK_H