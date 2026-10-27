#include "pch.h"
#include "platform.h"

Platform::Platform(const glm::vec3 position,
    const glm::vec3 scale,
    const engine::ref<engine::material> material,
    bool is_static)
{
    engine::ref<engine::cuboid> cube_shape = engine::cuboid::create(scale * 0.5f, false);

    engine::game_object_properties props;
    props.position = position;
    props.scale = glm::vec3(1.0f);
    props.meshes = { cube_shape->mesh() };
    props.type = 0;
    props.mass = 0.0f;//is_static ? 0.0f : 1.0f;
    props.is_static = is_static;
    props.bounding_shape = scale * 0.5f;
    props.restitution = 0.1f;
    props.friction = 0.8f;

    m_object = engine::game_object::create(props);
    m_material = material;
}

void Platform::update(const engine::timestep& time_step)
{
    // to-do: extend for moving platforms
}

void Platform::render(const engine::ref<engine::shader> shader) const
{
    m_material->submit(shader);
    engine::renderer::submit(shader, m_object);
}
