#pragma once
#include <engine.h>
#include <vector>

class GameEndVolume {
public:
    GameEndVolume(glm::vec3 position, glm::vec3 size);

    void on_update(engine::timestep ts);
    void on_render(const engine::ref<engine::shader>& shader);

    engine::ref<engine::game_object> object() const { return m_object; }
    bool check_overlap(glm::vec3 point);

private:
    struct Sparkle {
        engine::ref<engine::game_object> obj;
        engine::ref<engine::material> material;
        glm::vec3 velocity;
        float life;
    };

    engine::ref<engine::game_object> m_object;
    engine::ref<engine::material> m_material;
    engine::ref<engine::material> m_sparkle_material;

    glm::vec3 m_center;
    float m_emit_accumulator = 0.f;

    std::vector<Sparkle> m_sparkles;
    unsigned int m_max_particles = 100;

    void spawn_sparkle();
    void update_sparkles(float dt);
    void render_sparkles(const engine::ref<engine::shader>& shader);
};
