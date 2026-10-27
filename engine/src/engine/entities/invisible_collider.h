#pragma once
#include <engine.h>

class InvisibleCollider {
public:
    InvisibleCollider(glm::vec3 position, glm::vec3 scale);
    engine::ref<engine::game_object> object() const { return m_object; }

private:
    engine::ref<engine::game_object> m_object;
};

