#include "pch.h"
#include "cone.h"

engine::cone::cone(float radius, float height, uint32_t segments) {
    std::vector<mesh::vertex> vertices;
    std::vector<uint32_t> indices;

    glm::vec3 tip = { 0.f, height * 0.5f, 0.f };
    glm::vec3 base_center = { 0.f, -height * 0.5f, 0.f };

    float angle_step = 2.0f * PI / segments;

    // Store base vertex indices for cap
    std::vector<uint32_t> base_indices;

    // Side vertices (base ring)
    for (uint32_t i = 0; i <= segments; ++i) {
        float angle = i * angle_step;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;

        glm::vec3 base_vertex = { x, -height * 0.5f, z };

        vertices.push_back({ base_vertex, glm::normalize(glm::vec3(x, radius * radius / height, z)), {float(i) / segments, 0.f} });
        base_indices.push_back((uint32_t)vertices.size() - 1);
    }

    // Tip vertex
    vertices.push_back({ tip, glm::vec3(0.f, 1.f, 0.f), {0.5f, 1.f} });
    uint32_t tip_index = (uint32_t)vertices.size() - 1;

    // Side triangles
    for (uint32_t i = 0; i < segments; ++i) {
        indices.push_back(base_indices[i]);
        indices.push_back(base_indices[i + 1]);
        indices.push_back(tip_index);
    }

    // --- Bottom cap ---
    // Add center vertex
    vertices.push_back({ base_center, glm::vec3(0.f, -1.f, 0.f), {0.5f, 0.5f} });
    uint32_t center_index = (uint32_t)vertices.size() - 1;

    for (uint32_t i = 0; i < segments; ++i) {
        // Triangle in clockwise winding for front-facing bottom
        indices.push_back(center_index);
        indices.push_back(base_indices[i + 1]);
        indices.push_back(base_indices[i]);
    }

    m_mesh = mesh::create(vertices, indices);
}

engine::cone::~cone() {}

engine::ref<engine::cone> engine::cone::create(float radius, float height, uint32_t segments) {
    return std::make_shared<engine::cone>(radius, height, segments);
}
