//
// Created by User on 10/8/2025.
//

#ifndef LEARNVULKANRAII_RENDERER_UTILS_H
#define LEARNVULKANRAII_RENDERER_UTILS_H

#include <vulkan/vulkan_raii.hpp>

namespace LearnVulkanRAII::Utils
{
    bool glslToSPV(const vk::ShaderStageFlagBits shaderType,
                   std::string const& shaderCode,
                   std::vector<uint32_t>& spvShader);

    vk::raii::ShaderModule createShaderModule(vk::raii::Device const& device,
                                              vk::ShaderStageFlagBits shaderStage,
                                              std::string const& shaderText);

    std::string readFile(const std::string& filePath);
}

#endif //LEARNVULKANRAII_RENDERER_UTILS_H