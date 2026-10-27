#pragma once
#include "engine.h"
#include "engine/entities/shapes/cone.h"
#include "engine/entities/shapes/cylinder.h"
#include <random>
#include <unordered_map>

struct SpiritObject {
    engine::ref<engine::game_object> object;
    engine::ref<engine::material> material;
    float animation_time = 0.f;
};

class EnvironmentGenerator {
public:
    EnvironmentGenerator(float map_radius = 20.f, float clearing_radius = 6.f);

    void generate();                          // Clears and regenerates environment
    void update(float delta_time);           // Animate floating spirits
    void render(const engine::ref<engine::shader>& shader); // Draw all elements

    const std::vector<engine::ref<engine::game_object>>& trees() const { return m_trees; }
    const std::vector<engine::ref<engine::game_object>>& ruins() const { return m_ruins; }
    const std::vector<SpiritObject>& spirits() const { return m_spirits; }

    void set_tree_model_paths(const std::vector<std::string>& paths) { m_tree_model_paths = paths; }
    void set_ruin_model_paths(const std::vector<std::string>& paths) { m_ruin_model_paths = paths; }

    std::vector<engine::ref<engine::game_object>> EnvironmentGenerator::get_static_collidables() const {
        std::vector<engine::ref<engine::game_object>> result;
        result.insert(result.end(), m_trees.begin(), m_trees.end());
        result.insert(result.end(), m_ruins.begin(), m_ruins.end());
        return result;
    }


private:
    void generate_trees();
    void generate_ruins();
    void generate_spirits();

    engine::ref<engine::model> load_model(const std::string& path);

    float m_map_radius;
    float m_clearing_radius;

    std::vector<engine::ref<engine::game_object>> m_trees;
    std::vector<engine::ref<engine::game_object>> m_ruins;
    std::vector<SpiritObject> m_spirits;

    std::vector<std::string> m_tree_model_paths;
    std::vector<std::string> m_ruin_model_paths;

    std::unordered_map<std::string, engine::ref<engine::model>> m_loaded_models;

    std::default_random_engine m_rng;
};
