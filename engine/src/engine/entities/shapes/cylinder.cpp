#include "pch.h"
#include "cylinder.h"

engine::cylinder::cylinder(float radius, float height, uint32_t segments) {
    std::vector<mesh::vertex> vertices;
    std::vector<uint32_t> indices;

    float half_height = height * 0.5f;
    float angle_step = 2.0f * PI / segments;

    // Side vertices
    for (uint32_t i = 0; i <= segments; ++i) {
        float angle = i * angle_step;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glm::vec3 normal = glm::normalize(glm::normalize(glm::vec3(x, 0.f, z)));

        // Top and bottom vertex (per segment)
        vertices.push_back({ {x, half_height, z}, normal, {float(i) / segments, 1.f} });
        vertices.push_back({ {x, -half_height, z}, normal, {float(i) / segments, 0.f} });
    }

    // Side indices
    for (uint32_t i = 0; i < segments; ++i) {
        uint32_t top1 = i * 2;
        uint32_t bot1 = i * 2 + 1;
        uint32_t top2 = (i + 1) * 2;
        uint32_t bot2 = (i + 1) * 2 + 1;

        // Two triangles per quad
        indices.push_back(top1);
        indices.push_back(top2);
        indices.push_back(bot1);

        indices.push_back(top2);
        indices.push_back(bot2);
        indices.push_back(bot1);
    }

    // Center vertices for caps
    uint32_t center_top_index = static_cast<uint32_t>(vertices.size());
    vertices.push_back({ {0.f, half_height, 0.f}, {0.f, 1.f, 0.f}, {0.5f, 0.5f} });

    uint32_t center_bottom_index = static_cast<uint32_t>(vertices.size());
    vertices.push_back({ {0.f, -half_height, 0.f}, {0.f, -1.f, 0.f}, {0.5f, 0.5f} });

    // Top cap
    for (uint32_t i = 0; i < segments; ++i) {
        uint32_t top = i * 2;
        uint32_t next_top = ((i + 1) % segments) * 2;

        indices.push_back(center_top_index);
        indices.push_back(next_top);
        indices.push_back(top);
    }

    // Bottom cap
    for (uint32_t i = 0; i < segments; ++i) {
        uint32_t bot = i * 2 + 1;
        uint32_t next_bot = ((i + 1) % segments) * 2 + 1;

        indices.push_back(center_bottom_index);
        indices.push_back(bot);
        indices.push_back(next_bot);
    }

    m_mesh = mesh::create(vertices, indices);
}

engine::cylinder::~cylinder() {}

engine::ref<engine::cylinder> engine::cylinder::create(float radius, float height, uint32_t segments) {
    return std::make_shared<cylinder>(radius, height, segments);
}
