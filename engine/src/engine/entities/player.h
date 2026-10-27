#pragma once
#include <engine.h>
#include "engine/entities/game_object.h"

class Player {
public:
    Player(engine::ref<engine::game_object> object,
        engine::ref<engine::audio_manager> audio,
        engine::ref<engine::bullet_manager> physics_manager,
        engine::perspective_camera* camera);

    void on_update(const engine::timestep& ts, const glm::vec3& forward, const glm::vec3& right);
    void handle_input(const glm::vec3& forward, const glm::vec3& right);
    void handle_jump();

    void take_damage(int amount);
    void heal(int amount);
    int health() const { return m_health; }
    int max_health() const { return m_max_health; }

    void set_jump_powerup_multiplier(float multiplier, float duration) {
        m_jump_multiplier = multiplier;
        m_jump_powerup_timer = duration;
    }

    bool check_collision(engine::ref<engine::game_object> obj);

private:
    bool is_grounded() const;

    engine::ref<engine::game_object> m_object;
    engine::ref<engine::audio_manager> m_audio;
    engine::ref<engine::bullet_manager> m_physics_manager;
    engine::perspective_camera* m_camera = nullptr;

    bool m_jump_key_held = false;
    float m_jump_force = 5.0f;
    float m_ground_check_distance = 0.6f;

    float m_jump_powerup_timer = 0.f;
    float m_jump_multiplier = 1.f;

    int m_health = 100;
    int m_max_health = 100;
};


