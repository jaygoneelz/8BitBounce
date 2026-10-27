#pragma once
#include <engine.h>

namespace engine {
    class mesh;

    class cylinder {
    public:
        static ref<cylinder> create(float radius, float height, uint32_t segments);
        ~cylinder();
        const ref<mesh>& mesh() const { return m_mesh; }
        cylinder(float radius, float height, uint32_t segments);

    private:
        ref<engine::mesh> m_mesh;
    };

}
