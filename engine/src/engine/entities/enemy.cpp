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

    // Stable spawn-based roles: scouts see farther; hunters run faster; guards hit harder.
    m_archetype = int(std::abs(spawn_position.x * 13.f + spawn_position.z * 7.f)) % 3;
    m_detection_range = m_archetype == 0 ? 14.f : 9.f;
    m_speed = m_archetype == 1 ? 3.6f : 2.5f;
    glm::vec3 color = m_archetype == 0 ? glm::vec3(.9f,.65f,.2f) :
        m_archetype == 1 ? glm::vec3(.85f,.25f,.35f) : glm::vec3(.55f,.3f,.85f);
    m_material = engine::material::create(1.f, color, color, glm::vec3(.2f), 8.f);

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
    const float dt = float(ts);
    glm::vec3 delta = player_position - m_object->position();
    float distance = glm::length(glm::vec2(delta.x, delta.z));
    bool reachable = std::abs(delta.y) < 2.f;
    m_alert_timer += dt;
    // Periodic investigation makes the arena edge a temporary refuge, not a free win.
    if (m_state == EnemyState::Patrol && m_alert_timer >= 12.f) {
        m_alert_timer = 0.f;
        change_state(EnemyState::Investigate);
    }
    if (m_state == EnemyState::Recover) {
        m_object->set_velocity({0.f, m_object->velocity().y, 0.f});
        if (m_state_timer > .65f) change_state(EnemyState::Chase);
        return;
    }
    if (reachable && distance <= m_attack_range) change_state(EnemyState::Attack);
    else if (reachable && distance < m_detection_range) change_state(EnemyState::Chase);
    else if (m_state == EnemyState::Attack || (m_state == EnemyState::Chase && !reachable))
        change_state(EnemyState::Investigate);

    switch (m_state) {
    case EnemyState::Patrol:
        if (glm::distance(m_patrol_target, m_object->position()) < .5f || m_state_timer > 5.f)
            pick_new_patrol_point();
        move_toward(m_patrol_target, 1.5f, dt);
        break;
    case EnemyState::Investigate:
        move_toward(player_position, m_speed * .75f, dt);
        if (m_state_timer > 8.f) change_state(EnemyState::Patrol);
        break;
    case EnemyState::Chase:
        move_toward(player_position, m_speed, dt);
        break;
    case EnemyState::Attack:
        m_object->set_velocity({0.f, m_object->velocity().y, 0.f});
        if (m_attack_timer >= m_attack_cooldown) {
            attack_player(player);
            change_state(EnemyState::Recover);
        }
        break;
    default: break;
    }
}

void Enemy::move_toward(const glm::vec3& target, float speed, float dt)
{
    glm::vec3 direction = target - m_object->position();
    direction.y = 0.f;
    float distance = glm::length(direction);
    if (distance < .05f) return;
    direction /= distance;
    // Velocity is integrated by Bullet with the timestep; never move a fixed distance per frame.
    float safe_speed = glm::min(speed, distance / glm::max(dt, .001f));
    m_object->set_velocity({direction.x * safe_speed, m_object->velocity().y, direction.z * safe_speed});
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
        player->take_damage(m_archetype == 2 ? 15 : 10);
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

void Enemy::reset() {
    m_state = EnemyState::Patrol;
    m_alert_timer = m_state_timer = m_attack_timer = 0.f;
    m_object->set_position(m_spawn_position);
    m_object->set_velocity(glm::vec3(0.f));
    pick_new_patrol_point();
}
