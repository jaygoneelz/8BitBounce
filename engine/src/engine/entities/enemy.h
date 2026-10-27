#pragma once
#include <engine.h>

enum class EnemyState {
    Idle,
    Patrol,
    Chase,
    Attack,
    Dead
};

class Player; // forward declaration

class Enemy {
public:
    Enemy(const glm::vec3& spawn_position, float patrol_radius);

    void update(const engine::timestep& ts, const glm::vec3& player_position, engine::ref<Player>);
    void render(const engine::ref<engine::shader>& shader);

    engine::ref<engine::game_object> object() const { return m_object; }

private:
    void update_state(const engine::timestep& ts, const glm::vec3& player_position, engine::ref<Player>);
    void move_toward(const glm::vec3& target, float speed);
    void pick_new_patrol_point();
    void change_state(EnemyState new_state);
    void attack_player(engine::ref<Player> player);

private:
    engine::ref<engine::game_object> m_object;
    engine::ref<engine::material> m_material;

    EnemyState m_state = EnemyState::Idle;

    glm::vec3 m_spawn_position;
    glm::vec3 m_patrol_target;

    float m_patrol_radius = 5.f;
    float m_detection_range = 5.0f;
    float m_attack_range = 1.0f;
    float m_state_timer = 0.0f;
    float m_attack_timer = 0.0f;
    float m_attack_cooldown = 1.0f;

    bool m_alive = true;
};
