#pragma once

#include <engine.h>
#include "pch.h"

enum class RenderingType {
    Material,
    Texture
};

class Platform
{
public:
    Platform(const glm::vec3 position,
        const glm::vec3 scale,
        const engine::ref<engine::material> material,
        bool is_static = true);

    Platform(const glm::vec3 position,
        const glm::vec3 scale,
        const engine::ref<engine::texture_2d> texture,
        bool is_static = true);

    void update(const engine::timestep& time_step);
    void render(const engine::ref<engine::shader> shader) const;

    // Getters
    engine::ref<engine::game_object> object() const { return m_object; }
    engine::ref<engine::material> material() const { return m_material; }

private:
    engine::ref<engine::game_object> m_object;
    engine::ref<engine::material> m_material;
    engine::ref<engine::texture_2d> m_texture;
    RenderingType m_rendering_type;
};
