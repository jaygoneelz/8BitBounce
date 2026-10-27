#include "pch.h"
#include "moving_platform.h"

MovingPlatform::MovingPlatform(glm::vec3 start,
                               glm::vec3 end,
                               float speed,
                               engine::ref<engine::material> material,
                               glm::vec3 scale)
    : m_start(start), m_end(end), m_speed(speed), m_progress(0.0f), m_forward(true)
{
    m_direction = glm::normalize(end - start);
    m_platform = std::make_shared<Platform>(start, scale, material, false);
    m_last_position = m_platform->object()->position();
}

void MovingPlatform::update(const engine::timestep& ts)
{
    glm::vec3 current_position = m_platform->object()->position();
    m_platform_delta = current_position - m_last_position;
    m_last_position = current_position;

    float delta = float(ts) * m_speed;
    m_progress += (m_forward ? delta : -delta);

    // Clamp and reverse direction if needed
    if (m_progress > 1.0f)
    {
        m_progress = 1.0f;
        m_forward = false;
    }
    else if (m_progress < 0.0f)
    {
        m_progress = 0.0f;
        m_forward = true;
    }

    current_position = glm::mix(m_start, m_end, m_progress);
    m_platform->object()->set_position(current_position);
}

void MovingPlatform::render(const engine::ref<engine::shader>& shader) const
{
    m_platform->render(shader);
}

