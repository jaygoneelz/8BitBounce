#include "pch.h"
#include "game_end_volume.h"
#include <random>
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include <gl/GL.h>
#include <glm/gtx/norm.hpp>

GameEndVolume::GameEndVolume(glm::vec3 position, glm::vec3 size)
    : m_center(position)
{
    auto cyl = engine::cylinder::create(0.5f, 0.5f, 32);
    engine::game_object_properties props;
    props.position = position;
    props.scale = size;
    props.meshes = { cyl->mesh() };
    props.bounding_shape = size * 0.5f;
    props.is_static = true;

    m_material = engine::material::create(0.5f,
        glm::vec3(0.2f, 1.f, 1.f),
        glm::vec3(0.4f, 1.f, 1.f),
        glm::vec3(0.6f, 1.f, 1.f),
        0.3f);

    m_object = engine::game_object::create(props);

    // Preallocate sparkle slots
    m_sparkles.resize(m_max_particles);
    for (auto& s : m_sparkles) {
        auto sphere = engine::sphere::create(10, 20, 0.05f); // tiny sparkles

        engine::game_object_properties p;
        p.position = m_center;
        p.scale = glm::vec3(1.f);
        p.meshes = { sphere->mesh() };
        p.is_static = true;

        s.material = engine::material::create(1.f,
            glm::vec3(1.f),
            glm::vec3(1.f),
            glm::vec3(1.f),
            0.7f);
        s.material->set_transparency(0.7f);

        s.obj = engine::game_object::create(p);
        s.life = 0.f;
    }
}

void GameEndVolume::on_update(engine::timestep ts)
{
    float dt = (float)ts.seconds();
    m_emit_accumulator += dt;

    // Emit a new sparkle every frame with a slight delay for smoothness
    float spawn_interval = 0.02f;  // slower and smoother than 0.05

    while (m_emit_accumulator >= spawn_interval) {
        spawn_sparkle();
        m_emit_accumulator -= spawn_interval;
    }

    update_sparkles(dt);
}

void GameEndVolume::on_render(const engine::ref<engine::shader>& shader)
{
    auto gl_shader = std::dynamic_pointer_cast<engine::gl_shader>(shader);
    gl_shader->set_uniform("lighting_on", false);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    m_material->submit(shader);
    engine::renderer::submit(shader, m_object);
    render_sparkles(shader);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    gl_shader->set_uniform("lighting_on", true);
}

bool GameEndVolume::check_overlap(glm::vec3 point)
{
    glm::vec3 min = m_object->position() - m_object->bounding_shape();
    glm::vec3 max = m_object->position() + m_object->bounding_shape();
    return (point.x > min.x && point.x < max.x &&
        point.y > min.y && point.y < max.y &&
        point.z > min.z && point.z < max.z);
}

void GameEndVolume::spawn_sparkle()
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dir(-1.0f, 1.0f);
    std::uniform_real_distribution<float> speed(0.3f, 0.6f);  // gentler motion
    std::uniform_real_distribution<float> life(1.5f, 3.f);

    for (auto& s : m_sparkles)
    {
        if (s.life <= 0.f)
        {
            glm::vec3 direction;
            do {
                direction = glm::vec3(dir(rng), dir(rng), dir(rng));
            } while (glm::length2(direction) > 1.0f || glm::length2(direction) < 0.1f);

            direction = glm::normalize(direction);

            s.life = life(rng);
            s.velocity = direction * speed(rng);
            s.obj->set_position(m_center);
            break;
        }
    }
}

void GameEndVolume::update_sparkles(float dt)
{
    for (auto& s : m_sparkles)
    {
        if (s.life > 0.f)
        {
            s.life -= dt;
            glm::vec3 pos = s.obj->position() + s.velocity * dt;
            s.obj->set_position(pos);

            float alpha = glm::clamp(s.life / 2.f, 0.f, 1.f);
            s.material->set_transparency(alpha);
        }
    }
}

void GameEndVolume::render_sparkles(const engine::ref<engine::shader>& shader)
{
    for (auto& s : m_sparkles)
    {
        if (s.life > 0.f) {
            s.material->submit(shader);
            engine::renderer::submit(shader, s.obj);
        }
    }
}
