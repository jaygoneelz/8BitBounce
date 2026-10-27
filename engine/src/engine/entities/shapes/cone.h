#pragma once
#include <engine.h>

namespace engine {

    class mesh;

    class cone {
    public:
        static ref<cone> create(float radius, float height, uint32_t segments);
        ~cone();
        const ref<mesh>& mesh() const { return m_mesh; }
        cone(float radius, float height, uint32_t segments);

    private:
        ref<engine::mesh> m_mesh;
    };

}

