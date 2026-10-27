#pragma once
#include <engine.h>

namespace engine
{
    class quad
    {
    public:
        quad(glm::vec2 size = glm::vec2(2.f), bool flip_uv = false);
        ~quad();

        static ref<engine::quad> create(glm::vec2 size = glm::vec2(2.f), bool flip_uv = false);
        const ref<engine::mesh>& mesh() const { return m_mesh; }

    private:
        ref<engine::mesh> m_mesh;
    };
}

