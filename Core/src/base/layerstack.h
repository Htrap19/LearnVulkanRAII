//
// Created by User on 10/3/2025.
//

#ifndef LEARNVULKANRAII_LAYERSTACK_H
#define LEARNVULKANRAII_LAYERSTACK_H

#include "layer.h"

#include <vector>

#include "event/event.h"

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

    protected:
        virtual void onUpdate();
        virtual void onEvent(Event& e);

    private:
        LayerListType m_layers;
        size_t m_layerInsertIndex = 0;

        friend class Window;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_LAYERSTACK_H