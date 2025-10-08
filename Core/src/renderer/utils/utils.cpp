//
// Created by User on 10/8/2025.
//

#include "utils.h"
#include "base/utils.h"

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

#include <vulkan/vulkan.hpp>

#include <fstream>
#include <sstream>

namespace LearnVulkanRAII::Utils
{
    static EShLanguage translateShaderStage(vk::ShaderStageFlagBits shaderType)
    {
        switch (shaderType)
        {
            case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
            case vk::ShaderStageFlagBits::eTessellationControl: return EShLangTessControl;
            case vk::ShaderStageFlagBits::eTessellationEvaluation: return EShLangTessEvaluation;
            case vk::ShaderStageFlagBits::eGeometry: return EShLangGeometry;
            case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
            case vk::ShaderStageFlagBits::eCompute: return EShLangCompute;
            case vk::ShaderStageFlagBits::eRaygenNV: return EShLangRayGenNV;
            case vk::ShaderStageFlagBits::eAnyHitNV: return EShLangAnyHitNV;
            case vk::ShaderStageFlagBits::eClosestHitNV: return EShLangClosestHitNV;
            case vk::ShaderStageFlagBits::eMissNV: return EShLangMissNV;
            case vk::ShaderStageFlagBits::eIntersectionNV: return EShLangIntersectNV;
            case vk::ShaderStageFlagBits::eCallableNV: return EShLangCallableNV;
            case vk::ShaderStageFlagBits::eTaskNV: return EShLangTaskNV;
            case vk::ShaderStageFlagBits::eMeshNV: return EShLangMeshNV;

            default: ASSERT(false, "Unknown shader stage");
        }

        return EShLangVertex;
    }

    bool glslToSPV(const vk::ShaderStageFlagBits shaderType,
                   std::string const &shaderCode,
                   std::vector<uint32_t> &spvShader)
    {
        EShLanguage shaderStage = translateShaderStage(shaderType);

        const char* shaderStrings[1];
        shaderStrings[0] = shaderCode.data();

        glslang::TShader shader{shaderStage};
        shader.setStrings(shaderStrings, 1);

        EShMessages messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

        if (!shader.parse(GetDefaultResources(), 100, false, messages))
        {
            LOG("{}", shader.getInfoLog());
            LOG("{}", shader.getInfoDebugLog());
            return false;
        }

        glslang::TProgram program;
        program.addShader(&shader);

        // Program level processing
        if (!program.link(messages))
        {
            LOG("{}", program.getInfoLog());
            LOG("{}", program.getInfoDebugLog());
            return false;
        }

        glslang::GlslangToSpv(*program.getIntermediate(shaderStage), spvShader);
        return true;
    }

    vk::raii::ShaderModule createShaderModule(vk::raii::Device const &device,
                                              vk::ShaderStageFlagBits shaderStage,
                                              std::string const &shaderText)
    {
        std::vector<uint32_t> spvShader;
        bool result = glslToSPV(shaderStage, shaderText, spvShader);
        ASSERT(result, "Failed to convert glsl shader to spv");

        return vk::raii::ShaderModule(device, vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), spvShader));
    }

    std::string readFile(const std::string &filePath)
    {
        std::ifstream file(filePath);
        ASSERT(file.is_open(), "Unable to open the file!");

        std::stringstream ss;
        ss << file.rdbuf();

        return std::move(ss.str());
    }
}
