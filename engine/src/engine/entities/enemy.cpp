#include "pch.h"
#include "enemy.h"
#include "player.h"
#include <random>

Enemy::Enemy(const glm::vec3& spawn_position, float patrol_radius)
    : m_spawn_position(spawn_position), m_patrol_radius(patrol_radius)
{
    glm::vec3 cube_half_extents = glm::vec3(1.f);
    engine::ref<engine::cuboid> cube = engine::cuboid::create(cube_half_extents, false);
    m_attack_range += glm::max(cube_half_extents.x, glm::max(cube_half_extents.y, cube_half_extents.z));

    engine::game_object_properties props;
    props.position = spawn_position;
    props.scale = glm::vec3(1.0f);
    props.meshes = { cube->mesh() };
    props.is_static = false;
    props.bounding_shape = cube_half_extents;
    props.mass = 1.0f;
    props.restitution = 0.1f;
    props.friction = 0.6f;

    m_object = engine::game_object::create(props);

    m_material = engine::material::create(1.f, { 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, 1.f);

    pick_new_patrol_point();
    m_state = EnemyState::Patrol;
}

void Enemy::update(const engine::timestep& ts, const glm::vec3& player_position, engine::ref<Player> player)
{
    if (!m_alive) return;

    m_state_timer += (float)ts;
    m_attack_timer += (float)ts;

    update_state(ts, player_position, player);
}

void Enemy::update_state(const engine::timestep& ts, const glm::vec3& player_position, engine::ref<Player> player)
{
    float distance = glm::distance(player_position, m_object->position());

    switch (m_state) {
    case EnemyState::Idle:
        change_state(EnemyState::Patrol);
        break;

    case EnemyState::Patrol:
        if (distance < m_detection_range) {
            change_state(EnemyState::Chase);
        }
        else {
            if (glm::distance(m_patrol_target, m_object->position()) < 0.5f || m_state_timer > 5.0f) {
                pick_new_patrol_point();
            }
            else {
                move_toward(m_patrol_target, 1.5f);
            }
        }
        break;

    case EnemyState::Chase:
        if (distance > m_detection_range + 2.0f)
            change_state(EnemyState::Patrol);
        else if (distance <= m_attack_range)
            change_state(EnemyState::Attack);
        else
            move_toward(player_position, 2.5f);
        break;

    case EnemyState::Attack:
        if (distance > m_attack_range)
            change_state(EnemyState::Chase);
        else
            attack_player(player);
        break;

    default:
        break;
    }
}

void Enemy::move_toward(const glm::vec3& target, float speed)
{
    glm::vec3 direction = glm::normalize(target - m_object->position());
    m_object->set_position(m_object->position() + direction * speed * 0.01f);
    m_object->set_forward(direction);
}

void Enemy::pick_new_patrol_point()
{
    std::uniform_real_distribution<float> offset(-m_patrol_radius, m_patrol_radius);
    std::mt19937 rng(std::random_device{}());

    glm::vec3 new_target = m_spawn_position + glm::vec3(offset(rng), 0.f, offset(rng));
    m_patrol_target = new_target;
    m_state_timer = 0.0f;
}

void Enemy::change_state(EnemyState new_state)
{
    m_state = new_state;
    m_state_timer = 0.0f;
}

void Enemy::attack_player(engine::ref<Player> player)
{
    if (m_attack_timer >= m_attack_cooldown) {
        LOG_CORE_INFO("Enemy attacking player for 10 damage!");
        player->take_damage(10); 
        m_attack_timer = 0.0f;
    }
}

void Enemy::render(const engine::ref<engine::shader>& shader)
{
    if (m_alive) {
        m_material->submit(shader);
        engine::renderer::submit(shader, m_object);
    }
}
