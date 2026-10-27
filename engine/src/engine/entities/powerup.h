#pragma once
#include <engine.h>
#include <engine/entities/player.h>

enum class PowerUpType {
    Heal,
    Bounce
};

class PowerUp {
public:
    PowerUp(const std::string& model_path,
        const glm::vec3& position,
        PowerUpType type,
        engine::ref<engine::audio_manager> audio,
        engine::perspective_camera* camera);

    void update(const engine::timestep& ts);
    void render(const engine::ref<engine::shader>& shader) const;

    bool is_collected() const { return m_collected; }
    void collect();
    PowerUpType type() const { return m_type; }

    engine::ref<engine::game_object> object() const { return m_object; }

private:
    engine::ref<engine::model> m_model;
    engine::ref<engine::game_object> m_object;
    engine::PointLight m_light{};
    engine::ref<engine::audio_manager> m_audio;

    std::vector<engine::ref<engine::game_object>> m_glow_layers;
    std::vector<engine::ref<engine::material>> m_glow_materials;

    engine::perspective_camera* m_camera = nullptr;

    PowerUpType m_type;
    float m_rotation_angle = 0.0f;
    bool m_collected = false;
};
