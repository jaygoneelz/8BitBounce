#include "pch.h"
#include "Character.h"
#include "engine/entities/game_object.h"

engine::Character::Character(ref<game_object> object) : m_object(object) {}

engine::Character::~Character() {}

void engine::Character::move(glm::vec3 accelerate, double dt)
{
	m_object->set_acceleration(accelerate);
}

void engine::Character::turn(glm::vec3 torque, double dt)
{
	m_object->set_torque(torque);
}
