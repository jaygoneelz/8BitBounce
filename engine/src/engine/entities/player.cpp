#include "pch.h"
#include "player.h"
#include <engine.h>
#include <engine/events/key_event.h>
#include <btBulletDynamicsCommon.h>

Player::Player(engine::ref<engine::game_object> object,
    engine::ref<engine::audio_manager> audio,
    engine::ref<engine::bullet_manager> physics_manager,
    engine::perspective_camera* camera)
    : m_object(object), m_audio(audio), m_physics_manager(physics_manager), m_camera(camera) {
    if (m_audio) {}
    else {
        m_audio = engine::audio_manager::instance();
    }
}


void Player::on_update(const engine::timestep& ts, const glm::vec3& forward, const glm::vec3& right)
{
    handle_input(forward, right);
    handle_jump();

    // Example fall damage
    if (m_object->position().y < -10.f) {
        take_damage(25);
        m_object->set_position(glm::vec3(0.f, 10.f, 0.f));
    }

    // Debug key to test damage/heal
    if (engine::input::key_pressed(engine::key_codes::KEY_K)) take_damage(10);
    if (engine::input::key_pressed(engine::key_codes::KEY_H)) heal(10);

    if (m_jump_powerup_timer) {
        m_jump_powerup_timer -= ts;
        if (m_jump_powerup_timer <= 0) {
            m_jump_multiplier = 1.f;
        }
    }

}

void Player::handle_input(const glm::vec3& forward, const glm::vec3& right)
{
    glm::vec3 direction(0.f);

    if (engine::input::key_pressed(engine::key_codes::KEY_W))
        direction += forward;
    if (engine::input::key_pressed(engine::key_codes::KEY_S))
        direction -= forward;
    if (engine::input::key_pressed(engine::key_codes::KEY_D))
        direction += right;
    if (engine::input::key_pressed(engine::key_codes::KEY_A))
        direction -= right;

    if (glm::length(direction) > 0.01f)
        direction = glm::normalize(direction) * 5.0f;

    m_object->set_acceleration(direction);

    if (glm::length(direction) > 0.1f)
        m_object->set_forward(glm::normalize(direction));
}

void Player::handle_jump()
{
    bool jump_pressed = engine::input::key_pressed(engine::key_codes::KEY_SPACE);

    if (!jump_pressed)
        m_jump_key_held = false;

    if (jump_pressed && !m_jump_key_held && is_grounded())
    {
        glm::vec3 vel = m_object->velocity();
        m_object->set_velocity({ vel.x, m_jump_force  * m_jump_multiplier, vel.z });
        m_jump_key_held = true;

        if (m_audio)
            if (m_camera)
                m_audio->play_spatialised_sound("jump", m_camera->position(), m_object->position());
            else
                m_audio->play_spatialised_sound("jump", m_object->position(), m_object->position());

    }
}

bool Player::check_collision(engine::ref<engine::game_object> obj)
{
    float distance = glm::distance(m_object->position(), obj->position());
    return distance < 0.3f;
}

bool Player::is_grounded() const
{
    glm::vec3 pos = m_object->position();
    btVector3 from(pos.x, pos.y, pos.z);
    btVector3 to(pos.x, pos.y - m_ground_check_distance, pos.z);

    btCollisionWorld::ClosestRayResultCallback ray_callback(from, to);
    m_physics_manager->get_dynamics_world()->rayTest(from, to, ray_callback);

    return ray_callback.hasHit();
}

void Player::take_damage(int amount)
{
    m_health -= amount;
    if (m_health < 0)
        m_health = 0;
    LOG_CORE_TRACE("Ouch! HP:{}/{}",m_health,m_max_health);
    if (m_audio)
        if (m_camera)
            m_audio->play_spatialised_sound("damage", m_camera->position(), m_object->position());
        else
            m_audio->play_spatialised_sound("damage", m_object->position(), m_object->position());
}

void Player::heal(int amount)
{
    m_health += amount;
    if (m_health > m_max_health)
        m_health = m_max_health;
    //LOG_CORE_TRACE("Ahhh~ HP:{}/{}", m_health, m_max_health);
    if (m_audio)
        if (m_camera)
            m_audio->play_spatialised_sound("heal", m_camera->position(), m_object->position());
        else
            m_audio->play_spatialised_sound("heal", m_object->position(), m_object->position());
}
