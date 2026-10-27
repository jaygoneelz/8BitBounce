#include "pch.h"
#include "powerup.h"
#include <engine.h>
#include <gl/GL.h>

PowerUp::PowerUp(const std::string& model_path,
    const glm::vec3& position,
    PowerUpType type,
    engine::ref<engine::audio_manager> audio,
    engine::perspective_camera* camera)
    : m_type(type), m_audio(audio), m_camera(camera)
{
    if (m_audio) {}
    else {
        m_audio = engine::audio_manager::instance();
    }

    m_model = engine::model::create(model_path);

    std::vector<engine::ref<engine::texture_2d>> textures;
    glm::vec3 scale;

    switch (m_type) {
    case PowerUpType::Heal:
        textures = { engine::texture_2d::create("assets/textures/HealthPotion.png", false) };
        scale = glm::vec3(0.05f);
        break;
    case PowerUpType::Bounce:
        textures = { engine::texture_2d::create("assets/textures/BouncePotion.png", false) };
        scale = glm::vec3(0.05f);
        break;
    default:
        break;
    }

    engine::game_object_properties props;
    props.meshes = m_model->meshes();
    props.textures = textures;
    props.position = position;
    props.scale = scale;
    props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
    props.rotation_amount = 0.f;
    props.bounding_shape = glm::vec3(0.5f);
    props.type = 0;
    props.mass = 0.0f;
    props.is_static = true;

    m_object = engine::game_object::create(props);

    m_light.Position = position;
    m_light.Color = glm::vec3(0.8f,1.f,0.5f);
    m_light.AmbientIntensity = 0.7f;
    m_light.DiffuseIntensity = 0.7f;

    // --- Glow Mesh ---
    // Use a simple sphere or cube for glow

    auto glow_shape = engine::sphere::create(10, 20, 0.5f);

    int num_layers = 7;
    float base_scale = 0.1;
    float scale_step = 0.05f;
    float base_opacity = 0.05f;
    float opacity_decay = 0.005f;

    for (int i = 0; i < num_layers; ++i) {
        float scale = base_scale + i * scale_step;
        float alpha = base_opacity - i * opacity_decay;
        alpha = glm::clamp(alpha, 0.01f, 1.0f);

        auto material = engine::material::create(
            1.0f,
            glm::vec3(m_light.Color),
            glm::vec3(m_light.Color),
            glm::vec3(m_light.Color),
            0.1f
        );
        material->set_transparency(alpha);
        m_glow_materials.push_back(material);

        engine::game_object_properties glow_props;
        glow_props.position = position;
        glow_props.scale = glm::vec3(scale);
        glow_props.meshes = { glow_shape->mesh() };
        glow_props.is_static = true;

        auto glow_obj = engine::game_object::create(glow_props);
        m_glow_layers.push_back(glow_obj);
    }
}

void PowerUp::update(const engine::timestep& ts)
{
    if (m_collected) return;

    if (m_rotation_angle > glm::radians(360.f)) m_rotation_angle -= glm::radians(360.f);
    m_rotation_angle += glm::radians(45.f) * (float)ts;
    m_object->set_rotation_amount(m_rotation_angle);

    m_light.Position = m_object->position();

    float time_seconds = static_cast<float>(ts);
    float scale_factor = 1.0f + 0.2f * sin(time_seconds * 2.0f);

    for (size_t i = 0; i < m_glow_layers.size(); ++i) {
        auto base_scale = 1.0f + i * 0.1f;
        m_glow_layers[i]->set_scale(glm::vec3(base_scale * scale_factor));
        m_glow_layers[i]->set_position(m_object->position() + glm::vec3(0.f, 0.4f, 0.f));
    }
}

void PowerUp::render(const engine::ref<engine::shader>& shader) const
{
    if (m_collected) return;


    engine::renderer::submit(shader, m_object);

    auto gl_shader = std::dynamic_pointer_cast<engine::gl_shader>(shader);

    // Disable lighting and depth writes for glow
    gl_shader->set_uniform("lighting_on", false);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // GL_ONE_MINUS_SRC_ALPHA or GL_ONE for additive

    for (size_t i = 0; i < m_glow_layers.size(); ++i) {
        m_glow_materials[i]->submit(shader);
        engine::renderer::submit(shader, m_glow_layers[i]);
    }

    // Restore state
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    gl_shader->set_uniform("lighting_on", true);
}

void PowerUp::collect()
{
    m_collected = true;
    if (m_audio)
        if (m_camera)
            m_audio->play_spatialised_sound("pickup", m_camera->position(), m_object->position());
        else
            m_audio->play_spatialised_sound("pickup", m_object->position(), m_object->position());
}
