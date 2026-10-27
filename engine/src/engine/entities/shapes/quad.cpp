#include "pch.h"
#include "quad.h"

engine::quad::quad(glm::vec2 size, bool flip_uv)
{
    float w = size.x * 0.5f;
    float h = size.y * 0.5f;

    std::vector<mesh::vertex> vertices = {
        // position                  // normal         // tex coord
        { { -w, -h, 0.0f },         { 0.f, 0.f, 1.f }, { 0.f, flip_uv ? 1.f : 0.f } },
        { {  w, -h, 0.0f },         { 0.f, 0.f, 1.f }, { 1.f, flip_uv ? 1.f : 0.f } },
        { {  w,  h, 0.0f },         { 0.f, 0.f, 1.f }, { 1.f, flip_uv ? 0.f : 1.f } },
        { { -w,  h, 0.0f },         { 0.f, 0.f, 1.f }, { 0.f, flip_uv ? 0.f : 1.f } },
    };

    std::vector<uint32_t> indices = {
        0, 1, 2,
        0, 2, 3
    };

    m_mesh = engine::mesh::create(vertices, indices);
}

engine::quad::~quad() {}

engine::ref<engine::quad> engine::quad::create(glm::vec2 size, bool flip_uv)
{
    return std::make_shared<engine::quad>(size, flip_uv);
}

