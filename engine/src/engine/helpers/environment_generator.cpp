#include "pch.h"
#include "environment_generator.h"

EnvironmentGenerator::EnvironmentGenerator(float map_radius, float clearing_radius)
    : m_map_radius(map_radius), m_clearing_radius(clearing_radius), m_rng(std::random_device{}()) {
}

void EnvironmentGenerator::generate() {
    m_trees.clear();
    m_ruins.clear();
    m_spirits.clear();

    generate_trees();
    generate_ruins();
    generate_spirits();
}

engine::ref<engine::model> EnvironmentGenerator::load_model(const std::string& path) {
    if (m_loaded_models.find(path) == m_loaded_models.end()) {
        m_loaded_models[path] = engine::model::create(path);
    }
    return m_loaded_models[path];
}

void EnvironmentGenerator::generate_trees() {
    std::uniform_real_distribution<float> pos_dist(-m_map_radius, m_map_radius);
    std::uniform_real_distribution<float> scale_dist(0.5f, 1.5f);
    std::uniform_real_distribution<float> y_offset(0.0f, 0.3f);
    std::uniform_int_distribution<size_t> model_picker(0, m_tree_model_paths.size() - 1);

    for (int i = 0; i < 500; ++i) {
        glm::vec2 pos2D;
        float dist;
        do {
            pos2D = glm::vec2(pos_dist(m_rng), pos_dist(m_rng));
            dist = glm::length(pos2D);
        } while (dist < m_clearing_radius);

        std::string model_path = m_tree_model_paths[model_picker(m_rng)];
        auto model = load_model(model_path);

        float base_scale = 3.f / glm::max(model->size().x, glm::max(model->size().y, model->size().z));
        float random_scale = base_scale * scale_dist(m_rng);

        engine::game_object_properties props;
        props.meshes = model->meshes();
        props.textures = model->textures();
        props.position = glm::vec3(pos2D.x, 0.5f + y_offset(m_rng), pos2D.y);
        props.scale = glm::vec3(random_scale);
        glm::vec3 half_extents = model->size() * 0.5f;
        props.bounding_shape = half_extents * props.scale;
        props.is_static = true;
        props.type = 0;

        m_trees.push_back(engine::game_object::create(props));
    }
}

void EnvironmentGenerator::generate_ruins() {
    const float ruin_clear_buffer = 2.0f;
    const float effective_clearing_radius = m_clearing_radius + ruin_clear_buffer;

    std::uniform_real_distribution<float> pos_dist(-m_map_radius, m_map_radius);
    std::uniform_real_distribution<float> scale_dist(0.8f, 2.5f);
    std::uniform_int_distribution<size_t> model_picker(0, m_ruin_model_paths.size() - 1);

    for (int i = 0; i < 50; ++i) {
        glm::vec2 pos2D;
        float dist;

        // Reject positions that fall within the clearing
        do {
            pos2D = glm::vec2(pos_dist(m_rng), pos_dist(m_rng));
            dist = glm::length(pos2D);
        } while (dist < effective_clearing_radius);

        // Load random ruin model
        std::string model_path = m_ruin_model_paths[model_picker(m_rng)];
        auto model = load_model(model_path);

        float scale = scale_dist(m_rng);

        // Calculate bounding box extents
        glm::vec3 min_bound = glm::vec3(FLT_MAX);
        glm::vec3 max_bound = glm::vec3(-FLT_MAX);

        for (const auto& mesh : model->meshes()) {
            for (const auto& vertex : mesh->vertices()) {
                min_bound = glm::min(min_bound, vertex.position);
                max_bound = glm::max(max_bound, vertex.position);
            }
        }

        glm::vec3 half_extents = (max_bound - min_bound) * 0.5f;

        engine::game_object_properties props;
        props.meshes = model->meshes();
        props.textures = model->textures();
        props.position = glm::vec3(pos2D.x, 0.5f, pos2D.y);
        props.scale = glm::vec3(scale);
        props.bounding_shape = half_extents * scale;
        props.is_static = true;

        m_ruins.push_back(engine::game_object::create(props));
    }
}

void EnvironmentGenerator::generate_spirits() {
    std::uniform_real_distribution<float> pos_dist(-m_map_radius, m_map_radius);
    std::uniform_real_distribution<float> scale_dist(0.8f, 2.5f);
    std::uniform_real_distribution<float> alpha_dist(0.4f, 0.7f);
    std::uniform_real_distribution<float> color_dist(0.6f, 1.0f);

    for (int i = 0; i < 100; ++i) {
        glm::vec2 pos2D;
        float dist;
        do {
            pos2D = glm::vec2(pos_dist(m_rng), pos_dist(m_rng));
            dist = glm::length(pos2D);
        } while (dist < m_clearing_radius + 2.f);

        glm::vec3 position = { pos2D.x, 30.f, pos2D.y };

        bool is_cone = (i % 2 == 0);
        float base_radius = 2.f;
        float base_height = 4.f;

        auto mesh = is_cone
            ? engine::cone::create(base_radius, base_height, 16)->mesh()
            : engine::cylinder::create(base_radius, base_height, 20)->mesh();

        float scale = scale_dist(m_rng);

        engine::game_object_properties props;
        props.meshes = { mesh };
        props.position = position;
        props.scale = glm::vec3(scale);
        props.is_static = true;
        props.mass = 0.f;
        props.restitution = 0.2f;
        props.friction = 1.0f;

        auto object = engine::game_object::create(props);

        glm::vec3 color = {
            color_dist(m_rng),
            color_dist(m_rng) * 0.7f,
            color_dist(m_rng)
        };

        auto material = engine::material::create(1.0f, color, color, color, 0.1f);
        material->set_transparency(alpha_dist(m_rng));

        m_spirits.push_back({ object, material, 0.f });
    }
}

void EnvironmentGenerator::update(float delta_time) {
    for (auto& spirit : m_spirits) {
        spirit.animation_time += delta_time;

        float hover_offset = 0.5f * sin(spirit.animation_time * 1.5f);
        glm::vec3 base_pos = spirit.object->position();
        base_pos.y = 5.f + hover_offset;

        spirit.object->set_position(base_pos);
        spirit.object->set_rotation_amount(spirit.animation_time * 0.5f);
    }
}

void EnvironmentGenerator::render(const engine::ref<engine::shader>& shader) {
    for (auto& tree : m_trees)
        engine::renderer::submit(shader, tree);

    for (auto& ruin : m_ruins)
        engine::renderer::submit(shader, ruin);

    std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", false);
    for (auto& spirit : m_spirits) {
        spirit.material->submit(shader);
        engine::renderer::submit(shader, spirit.object);
    }
    std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("lighting_on", true);
}
