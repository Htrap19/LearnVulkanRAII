//
// Created by User on 10/3/2025.
//

#include "layerstack.h"

#include <algorithm>

namespace LearnVulkanRAII
{
    LayerStack::LayerStack()
    {
    }

    LayerStack::~LayerStack()
    {
        for (const auto& layer : m_layers)
        {
            layer->onDetach();
        }
    }

    void LayerStack::pushLayer(const Layer::Shared& layer)
    {
        m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
        m_layerInsertIndex++;
        layer->onAttach();
    }

    void LayerStack::popLayer(const Layer::Shared& layer)
    {
        auto it = std::find(m_layers.begin(), m_layers.begin() + m_layerInsertIndex, layer);
        if (it != m_layers.begin() + m_layerInsertIndex)
        {
            layer->onDetach();
            m_layers.erase(it);
            m_layerInsertIndex--;
        }
    }

    void LayerStack::pushOverlay(const Layer::Shared& layer)
    {
        m_layers.emplace_back(layer);
        layer->onAttach();
    }

    void LayerStack::popOverlay(const Layer::Shared& layer)
    {
        auto it = std::find(m_layers.begin() + m_layerInsertIndex, m_layers.end(), layer);
        if (it != m_layers.end())
        {
            layer->onDetach();
            m_layers.erase(it);
        }
    }

    void LayerStack::onUpdate()
    {
        for (const auto& layer : m_layers)
        {
            layer->onUpdate();
        }
    }
} // LearnVulkanRAII