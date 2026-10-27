#pragma once
#include "engine/core.h"
#include "engine/renderer/vertex_array.h"
#include "engine/renderer/shader.h"
#include "engine/entities/game_object.h"

namespace engine {

    class bounding_box_renderer {
    public:
        static void init();
        static void draw(const ref<shader>& shader, const glm::vec3& position, const glm::vec3& half_extents, const glm::vec3& color);
        static void draw(const ref<shader>& shader, const ref<game_object>& obj, const glm::vec3& color);

    private:
        static ref<vertex_array> s_vertex_array;
    };

}
