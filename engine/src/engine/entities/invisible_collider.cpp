#include "pch.h"
#include "invisible_collider.h"

InvisibleCollider::InvisibleCollider(glm::vec3 position, glm::vec3 scale)
{
    engine::ref<engine::cuboid> shape = engine::cuboid::create(scale * 0.5f, false);

    engine::game_object_properties props;
    props.position = position;
    props.scale = glm::vec3(1.0f); // physics shape handles actual size
    props.meshes = { shape->mesh() };
    props.is_static = true;
    props.mass = 0.0f;
    props.type = 0;
    props.bounding_shape = scale * 0.5f;
    props.friction = 1.0f;
    props.restitution = 0.0f;

    m_object = engine::game_object::create(props);
}

