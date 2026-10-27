#pragma once

#include "platform.h"

class MovingPlatform {
public:
    MovingPlatform(glm::vec3 start,
                   glm::vec3 end,
                   float speed,
                   engine::ref<engine::material> material,
                   glm::vec3 scale);

    void update(const engine::timestep& ts);
    void render(const engine::ref<engine::shader>& shader) const;

    engine::ref<engine::game_object> object() const { return m_platform->object(); }

    glm::vec3 platform_delta() const { return m_platform_delta; }

private:
    engine::ref<Platform> m_platform;
    glm::vec3 m_start;
    glm::vec3 m_end;
    glm::vec3 m_direction;

    glm::vec3 m_last_position;
    glm::vec3 m_platform_delta;

    float m_speed;
    float m_progress;
    bool m_forward;
};
