//
// Created by User on 10/6/2025.
//

#ifndef LEARNVULKANRAII_MESH_H
#define LEARNVULKANRAII_MESH_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace LearnVulkanRAII
{
    struct Transform
    {
        glm::vec3 translate;
        glm::vec3 rotate;
        glm::vec3 scale;

        glm::mat4 toMat4() const
        {
            auto t = glm::translate(glm::mat4(1.0f), translate);
            auto r = glm::toMat4(glm::quat(rotate));
            auto s = glm::scale(glm::mat4(1.0f), scale);

            return t * r * s;
        }
    };

    struct Vertex
    {
        glm::vec3 position;
    };

    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        size_t getVerticesCount() const { return vertices.size(); }
        size_t getIndicesCount() const { return indices.size(); }

        size_t getVerticesSizeInBytes() const { return getVerticesCount() * sizeof(Vertex); }
        size_t getIndicesSizeInBytes() const { return getIndicesCount() * sizeof(uint32_t); }

        size_t getFaceCount() const { return indices.size() / 3; }

        void applyTransform(const Transform& transform)
        {
            const auto model = transform.toMat4();
            for (auto& v : vertices)
            {
                v.position = model * glm::vec4(v.position, 1.0f);
            }
        }
    };
}

#endif //LEARNVULKANRAII_MESH_H