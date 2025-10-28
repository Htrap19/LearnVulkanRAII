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
#include "renderercommandpool.h"

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

    struct InternalVertex
    {
        int objectMetadataIndex = 0;
    };

    struct BatchAllocationInfo
    {
    public:
        size_t batchSize = 0;
        PrimitiveType primType = PrimitiveType::TRIANGLES;

        size_t modelCount = 0;

    public:
        BatchAllocationInfo() = default;
        BatchAllocationInfo(size_t batchSize, size_t modelsCount)
            : batchSize(batchSize), modelCount(modelsCount)
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
            modelsSize = modelCount * sizeof(ObjectMetadata);
            internalVerticesSize = verticesSize + sizeof(InternalVertex);
        }

        size_t getVertexCount() const { return vertexCount; }
        size_t getIndexCount() const { return indexCount; }
        size_t getVerticesSizeInBytes() const { return verticesSize; }
        size_t getIndicesSizeInBytes() const { return indicesSize; }
        size_t getModelsSizeInBytes() const { return modelsSize; }
        size_t getInternalVertexSizeInBytes() const { return internalVerticesSize; }

    private:
        size_t vertexCount = 0;
        size_t indexCount = 0;

        // In bytes
        size_t verticesSize = 0;
        size_t indicesSize = 0;
        size_t modelsSize = 0;
        size_t internalVerticesSize = 0;
    };

    struct LocalTransferSpace
    {
        Vertex* vertices = nullptr;
        uint32_t* indices = nullptr;
        ObjectMetadata* objectMetadata = nullptr;
        InternalVertex* internalVertices = nullptr;
        BatchAllocationInfo batchInfo;

        size_t currentVertexCount = 0;
        size_t currentIndexCount = 0;
        size_t currentObjectMetadataCount = 0;

        LocalTransferSpace() = default;
        explicit LocalTransferSpace(const BatchAllocationInfo& allocationBatchInfo)
            : batchInfo(allocationBatchInfo)
        {
            allocate();
        }

        ~LocalTransferSpace()
        {
            deAllocate();
        }

        void setBatchInfo(const BatchAllocationInfo& allocationBatchInfo)
        {
            batchInfo = allocationBatchInfo;

            allocate();
        }

        void allocate()
        {
            ASSERT(batchInfo.batchSize > 0, "Invalid batchSize!");

            deAllocate();

            vertices = new Vertex[batchInfo.getVertexCount()];
            indices = new uint32_t[batchInfo.getIndexCount()];
            objectMetadata = new ObjectMetadata[batchInfo.modelCount];
            internalVertices = new InternalVertex[batchInfo.getVertexCount()];
        }

        void deAllocate()
        {
            if (vertices != nullptr)
                delete[] vertices;
            if (indices != nullptr)
                delete[] indices;
            if (objectMetadata != nullptr)
                delete[] objectMetadata;
            if (internalVertices != nullptr)
                delete[] internalVertices;

            vertices = nullptr;
            indices = nullptr;
            objectMetadata = nullptr;
            internalVertices = nullptr;

            resetCurrentCounts();
        }

        size_t getCurrentVerticesSizeInBytes() const { return currentVertexCount * sizeof(Vertex); }
        size_t getCurrentIndicesSizeInBytes() const { return currentIndexCount * sizeof(uint32_t); }
        size_t getCurrentObjectMetadataSizeInBytes() const { return currentObjectMetadataCount * sizeof(ObjectMetadata); }
        size_t getCurrentIntervalVerticesSizeInBytes() const { return currentVertexCount * sizeof(InternalVertex); }

        size_t getCurrentFaceCounts() const { return currentIndexCount / 3; }

        void resetCurrentCounts()
        {
            currentVertexCount = 0;
            currentIndexCount = 0;
            currentObjectMetadataCount = 0;
        }
    };

    struct FrameContext
    {
        uint32_t imageIndex; // swapchain acquired image index
        Utils::Optional<vk::raii::Semaphore> imageAvailableSemaphore;
        Utils::Optional<vk::raii::Semaphore> renderFinishedSemaphore;
        Utils::Optional<vk::raii::Fence> inFlightFence;
        std::vector<vk::raii::Semaphore> batchSemaphores;

        size_t drawCallCount = 0;
        bool isLastDrawCall = false;

        void resetCounts() { drawCallCount = 0, isLastDrawCall = false; }
    };

    struct InFlightFrameManager
    {
    public:
        std::vector<FrameContext> frames;

    public:
        InFlightFrameManager() = default;
        explicit InFlightFrameManager(uint32_t maxFramesInFlight)
            : maxFramesInFlight(maxFramesInFlight)
        {
            frames.reserve(maxFramesInFlight);
        }

        uint32_t getCurrentFrameIndex() const { return currentFrameIndex; }
        void nextFrame() { currentFrameIndex = (currentFrameIndex + 1) % maxFramesInFlight; }
        const FrameContext& getCurrentFrameContext() const { return frames[currentFrameIndex]; }
        FrameContext& getCurrentFrameContext() { return frames[currentFrameIndex]; }

    private:
        uint32_t currentFrameIndex = 0;
        uint32_t maxFramesInFlight = 0;
    };

    struct RendererStatistics
    {
        size_t totalDrawCallsCount = 0;
        size_t totalVertexCount = 0;
        size_t totalIndexCount = 0;

        [[nodiscard]] size_t getTotalFaceCount() const { return totalIndexCount / 3; }
        void reset() { memset(this, 0, sizeof(RendererStatistics)); }
    };

    class Renderer
    {
    public:
        DEFINE_SMART_POINTER_HELPERS(Renderer)

    public:
        explicit Renderer(const GraphicsContext::Shared& graphicsContext);

        void beginFrame(const CameraViewData& cameraData);
        void beginFrame(const SwapchainFramebuffer::Shared& framebuffer, const CameraViewData& cameraData);
        void endFrame();

        void drawMesh(const Mesh& mesh, const Transform& transform);

        void resize(uint32_t width, uint32_t height);

        void setBatchSize(size_t batchSize);
        [[nodiscard]] size_t getBatchSize() const;
        [[nodiscard]] const RendererStatistics& getStats() const;

        [[nodiscard]] const vk::raii::RenderPass& getRenderPass() const;

        // TODO: Need to create a 'create' function for renderer
        // static Shared create(...);

    private:
        void init();

        void createRenderPass();
        void createDescriptorSetLayout();
        void createGraphicsPipeline();
        void createCommandPools();
        void createSyncObjects();

        void allocateLocalTransferSpace();
        void createBuffers();
        void createDescriptorPool();
        void allocateDescriptorSets();
        void createDefaultFramebuffer();

        void recordCommandBuffer(const Framebuffer::Shared& fb) const;

        void draw();
        void presentFrame();

    private:
        GraphicsContext::Shared m_graphicsContext;

        Utils::Optional<vk::raii::RenderPass> m_renderPass;
        Utils::Optional<vk::raii::PipelineLayout> m_pipelineLayout;
        Utils::Optional<vk::raii::Pipeline> m_graphicsPipeline;
        std::vector<RendererCommandPool::Shared> m_commandPools;
        Utils::Optional<vk::raii::DescriptorSetLayout> m_descriptorSetLayout;
        Utils::Optional<vk::raii::DescriptorPool> m_descriptorPool;
        std::vector<vk::raii::DescriptorSet> m_descriptorSet;

        SwapchainFramebuffer::Shared m_defaultFramebuffer;
        SwapchainFramebuffer::Shared m_framebuffer;
        Buffer::Shared m_vertexBuffer;
        Buffer::Shared m_indexBuffer;
        Buffer::Shared m_cameraViewDataBuffer;
        Buffer::Shared m_objectMetadataBuffer;
        Buffer::Shared m_internalVertexBuffer;

        BatchAllocationInfo m_allocationBatchInfo;
        LocalTransferSpace m_localTransferSpace;
        InFlightFrameManager m_inFlightFrameManager;
        RendererStatistics m_stats;
    };
} // LearnVulkanRAII

#endif //LEARNVULKANRAII_RENDERER_H