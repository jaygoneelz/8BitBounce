#include "pch.h"
#include "bounding_box_renderer.h"
#include "engine/renderer/renderer_api.h"
#include "engine/renderer/buffer.h"
#include "platform/opengl/gl_shader.h"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

    ref<vertex_array> bounding_box_renderer::s_vertex_array = nullptr;

    void bounding_box_renderer::init()
    {
        // 8 corners of a unit cube centered at origin
        float vertices[] = {
            -1, -1, -1,
             1, -1, -1,
             1, -1,  1,
            -1, -1,  1,
            -1,  1, -1,
             1,  1, -1,
             1,  1,  1,
            -1,  1,  1,
        };

        uint32_t indices[] = {
            0, 1, 1, 2, 2, 3, 3, 0, // bottom face
            4, 5, 5, 6, 6, 7, 7, 4, // top face
            0, 4, 1, 5, 2, 6, 3, 7  // sides
        };

        auto vbo = vertex_buffer::create(vertices, sizeof(vertices));
        vbo->layout({ { e_shader_data_type::float3, "a_position" } });

        auto ibo = index_buffer::create(indices, sizeof(indices) / sizeof(uint32_t));

        s_vertex_array = vertex_array::create();
        s_vertex_array->add_buffer(vbo);
        s_vertex_array->add_buffer(ibo);
    }

    void bounding_box_renderer::draw(const ref<shader>& shader, const glm::vec3& position, const glm::vec3& half_extents, const glm::vec3& color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
        transform = glm::scale(transform, half_extents);

        auto gl_shader = std::dynamic_pointer_cast<engine::gl_shader>(shader);
        gl_shader->bind();
        gl_shader->set_uniform("u_transform", transform);
        gl_shader->set_uniform("u_color", color); // make sure your shader supports this

        renderer_api::line_width(2.0f);
        renderer_api::draw_indexed_lines(s_vertex_array);
    }

    void bounding_box_renderer::draw(const ref<shader>& shader, const ref<game_object>& obj, const glm::vec3& color)
    {
        draw(shader, obj->position(), obj->bounding_shape(), color);
    }

}
