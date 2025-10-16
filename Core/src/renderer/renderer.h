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

    struct CameraViewData
    {
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
    };

    struct ObjectMetadata
    {
        glm::mat4 model = glm::mat4(1.0f);
    };

    struct SceneUniformData
    {
        CameraViewData cameraView;
        ObjectMetadata objectsMetadata[32];
    };

    struct AllocationBatchInfo
    {
    public:
        size_t batchSize = 0;
        PrimitiveType primType = PrimitiveType::TRIANGLES;

        /* TODO: Need to implement a way by which we can configure the model count,
         * which will involve shader code modification programmatically */
        size_t modelsCount = 0;

    public:
        AllocationBatchInfo() = default;
        AllocationBatchInfo(size_t batchSize, size_t modelsCount)
            : batchSize(batchSize), modelsCount(modelsCount)
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
        size_t getVerticesSizeInBytes() const { return verticesSize; }
        size_t getIndicesSizeInBytes() const { return indicesSize; }

    private:
        size_t vertexCount = 0;
        size_t indexCount = 0;

        // In bytes
        size_t verticesSize = 0;
        size_t indicesSize = 0;
    };

    struct LocalTransferSpace
    {
        Vertex* vertices = nullptr;
        uint32_t* indices = nullptr;
        SceneUniformData* sceneUniformData = nullptr;
        AllocationBatchInfo batchInfo;

        size_t currentVertexCount = 0;
        size_t currentIndexCount = 0;
        size_t currentObjectMetadataCount = 0;

        size_t minUniformBufferAlignment = 0;
        size_t maxSceneUniformDataCount = 0;
        size_t uniformBufferAlignment = 0;

        LocalTransferSpace() = default;
        explicit LocalTransferSpace(const AllocationBatchInfo& allocationBatchInfo,
            size_t minUniformBufferAlignment,
            size_t maxSceneUniformDataCount)
            : batchInfo(allocationBatchInfo),
            minUniformBufferAlignment(minUniformBufferAlignment),
            maxSceneUniformDataCount(maxSceneUniformDataCount)
        {
            allocate();
        }

        ~LocalTransferSpace()
        {
            deAllocate();
        }

        void setBatchInfo(const AllocationBatchInfo& allocationBatchInfo)
        {
            setBatchInfo(allocationBatchInfo, minUniformBufferAlignment, maxSceneUniformDataCount);
        }

        void setBatchInfo(const AllocationBatchInfo& allocationBatchInfo,
            size_t minUniformBufferOffsetAlignment,
            size_t maxSceneUniformDataCount)
        {
            batchInfo = allocationBatchInfo;
            this->minUniformBufferAlignment = minUniformBufferOffsetAlignment;
            this->maxSceneUniformDataCount = maxSceneUniformDataCount;

            allocate();
        }

        void allocate()
        {
            ASSERT(batchInfo.batchSize > 0, "Invalid batchSize!");

            deAllocate();

            vertices = new Vertex[batchInfo.getVertexCount()];
            indices = new uint32_t[batchInfo.getIndexCount()];

            uniformBufferAlignment = calculateUniformBufferAlignment();
            sceneUniformData = static_cast<SceneUniformData *>(_aligned_malloc(
                getTotalSceneUniformDataSizeInBytes(), minUniformBufferAlignment));
        }

        void deAllocate()
        {
            if (vertices != nullptr)
                delete[] vertices;
            if (indices != nullptr)
                delete[] indices;
            if (sceneUniformData != nullptr)
                _aligned_free(sceneUniformData);

            vertices = nullptr;
            indices = nullptr;
            sceneUniformData = nullptr;

            resetCurrentCounts();
        }

        size_t calculateUniformBufferAlignment() const
        {
            return (sizeof(SceneUniformData) + minUniformBufferAlignment - 1) & ~(minUniformBufferAlignment - 1);
        }

        size_t getCurrentVerticesSizeInBytes() const { return currentVertexCount * sizeof(Vertex); }
        size_t getCurrentIndicesSizeInBytes() const { return currentIndexCount * sizeof(uint32_t); }

        size_t getTotalSceneUniformDataSizeInBytes() const
        { return uniformBufferAlignment * maxSceneUniformDataCount; }

        size_t getCurrentFaceCounts() const { return currentIndexCount / 3; }

        void resetCurrentCounts()
        {
            currentVertexCount = 0;
            currentIndexCount = 0;
            currentObjectMetadataCount = 0;
        }
    };

    class Renderer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Renderer)

    public:
        explicit Renderer(const GraphicsContext::Shared& graphicsContext);

        void beginFrame(const Framebuffer::Shared& framebuffer, const CameraViewData& cameraData);
        void endFrame();

        void drawMesh(const Mesh& mesh, const Transform& transform);

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

        void allocateLocalTransferSpace();
        void createBuffers();
        void createDescriptorPool();
        void allocateDescriptorSets();

        void recordCommandBuffer(const vk::raii::CommandBuffer& cb, const vk::raii::Framebuffer& fb, size_t imgIdx) const;

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
        Buffer::Shared m_sceneUniformDataBuffer;

        AllocationBatchInfo m_allocationBatchInfo;
        LocalTransferSpace m_localTransferSpace;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_RENDERER_H