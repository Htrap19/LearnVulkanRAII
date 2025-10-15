//
// Created by User on 10/6/2025.
//

#ifndef LEARNVULKANRAII_RENDERER_H
#define LEARNVULKANRAII_RENDERER_H

#include "base/utils.h"
#include "base/graphicscontext.h"

#include "mesh/mesh.h"

#include "framebuffer.h"
#include "buffer.h"

#include <vulkan/vulkan_raii.hpp>

namespace LearnVulkanRAII
{
    // As per current implementation, only triangles are supported as a rendering primitive
    enum class PrimitiveType
    {
        TRIANGLES
    };

    struct AllocationBatchInfo
    {
    public:
        size_t batchSize = 0;
        PrimitiveType primType = PrimitiveType::TRIANGLES;

    public:
        AllocationBatchInfo() = default;
        AllocationBatchInfo(size_t batchSize)
            : batchSize(batchSize)
        {
            calculate();
        }

        void calculate()
        {
            ASSERT(primType == PrimitiveType::TRIANGLES, "Invalid PrimitiveType!");

            vertexCount = batchSize * 3;
            indexCount = batchSize * 3;

            verticesSize = vertexCount * sizeof(Vertex);
            indicesSize = indexCount * sizeof(uint32_t);
        }

        size_t getVertexCount() const { return vertexCount; }
        size_t getIndexCount() const { return indexCount; }
        size_t getVerticesSize() const { return verticesSize; }
        size_t getIndicesSize() const { return indicesSize; }

    private:
        size_t vertexCount = 0;
        size_t indexCount = 0;

        // In bytes
        size_t verticesSize = 0;
        size_t indicesSize = 0;
    };

    struct LocalAllocation
    {
        Vertex* vertices = nullptr;
        uint32_t* indices = nullptr;
        AllocationBatchInfo batchInfo;

        size_t currentVertexCount = 0;
        size_t currentIndexCount = 0;

        LocalAllocation() = default;
        explicit LocalAllocation(const AllocationBatchInfo& allocationBatchInfo)
            : batchInfo(allocationBatchInfo)
        {
            allocate();
        }

        ~LocalAllocation()
        {
            deAllocate();
        }

        void setBatchInfo(const AllocationBatchInfo& allocationBatchInfo)
        {
            this->batchInfo = allocationBatchInfo;

            allocate();
        }

        void allocate()
        {
            ASSERT(batchInfo.batchSize > 0, "Invalid batchSize!");

            deAllocate();
            vertices = new Vertex[batchInfo.getVertexCount()];
            indices = new uint32_t[batchInfo.getIndexCount()];
        }

        void deAllocate()
        {
            if (vertices != nullptr)
                delete[] vertices;
            if (indices != nullptr)
                delete[] indices;

            vertices = nullptr;
            indices = nullptr;
            resetCurrentCounts();
        }

        size_t getCurrentVerticesSizeInBytes() const { return currentVertexCount * sizeof(Vertex); }
        size_t getCurrentIndicesSizeInBytes() const { return currentIndexCount * sizeof(uint32_t); }

        size_t getCurrentFaceCounts() const { return currentIndexCount / 3; }

        void resetCurrentCounts()
        {
            currentVertexCount = 0;
            currentIndexCount = 0;
        }
    };

    struct CameraViewData
    {
        glm::mat4 projection;
        glm::mat4 view;
    };

    class Renderer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Renderer)

    public:
        explicit Renderer(const GraphicsContext::Shared& graphicsContext);

        void beginFrame(const Framebuffer::Shared& framebuffer, const CameraViewData& cameraData);
        void endFrame();

        void drawMesh(const Mesh& mesh);

        void resize(uint32_t width, uint32_t height);

        void setBatchSize(size_t batchSize);
        size_t getBatchSize() const;

        [[nodiscard]] const vk::raii::RenderPass& getRenderPass() const;

        // TODO: Need to create a 'create' function for renderer
        // static Shared create(...);

    private:
        void init();

        void createRenderPass();
        void createDescriptorSetLayout();
        void createGraphicsPipeline();
        void allocateCommandBuffers();
        void createSyncObjects();

        void createBuffers();
        void createDescriptorPool();
        void allocateDescriptorSets();

        void recordCommandBuffer(const vk::raii::CommandBuffer& cb, const vk::raii::Framebuffer& fb) const;

        void drawFrame();

    private:
        GraphicsContext::Shared m_graphicsContext;

        Utils::Optional<vk::raii::RenderPass> m_renderPass;
        Utils::Optional<vk::raii::PipelineLayout> m_pipelineLayout;
        Utils::Optional<vk::raii::Pipeline> m_graphicsPipeline;
        std::vector<vk::raii::CommandBuffer> m_commandBuffers;
        Utils::Optional<vk::raii::Semaphore> m_imageAvailableSemaphore;
        Utils::Optional<vk::raii::Semaphore> m_renderFinishedSemaphore;
        Utils::Optional<vk::raii::Fence> m_inFlightFence;
        Utils::Optional<vk::raii::DescriptorSetLayout> m_descriptorSetLayout;
        Utils::Optional<vk::raii::DescriptorPool> m_descriptorPool;
        std::vector<vk::raii::DescriptorSet> m_descriptorSet;

        Framebuffer::Shared m_framebuffer;
        Buffer::Shared m_vertexBuffer;
        Buffer::Shared m_indexBuffer;
        Buffer::Shared m_cameraViewDataBuffer;

        AllocationBatchInfo m_allocationBatchInfo;
        LocalAllocation m_localAllocation;
        CameraViewData m_cameraViewData;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_RENDERER_H