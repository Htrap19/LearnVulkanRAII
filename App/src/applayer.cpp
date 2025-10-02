//
// Created by User on 10/3/2025.
//

#include "applayer.h"

#include <print>

AppLayer::AppLayer()
{
    std::println(__FUNCTION__);
}

void AppLayer::onAttach()
{
    std::println(__FUNCTION__);
}

void AppLayer::onDetach()
{
    std::println(__FUNCTION__);
}

void AppLayer::onUpdate()
{
    std::print(".");
}
