#include "pch.h"
#include "map_generator.h"

MapGenerator::MapGenerator(std::vector<engine::ref<engine::game_object>>& world_objects,
    std::vector<std::shared_ptr<Platform>>& platforms,
    std::vector<std::shared_ptr<MovingPlatform>>& moving_platforms,
    const engine::ref<engine::texture_2d>& platform_texture)
    : m_world_objects(world_objects),
    m_platforms(platforms),
    m_moving_platforms(moving_platforms),
    m_platform_texture(platform_texture)
{
    m_rendering_type = RenderingType::Texture;
}

MapGenerator::MapGenerator(std::vector<engine::ref<engine::game_object>>& world_objects,
    std::vector<std::shared_ptr<Platform>>& platforms,
    std::vector<std::shared_ptr<MovingPlatform>>& moving_platforms,
    const engine::ref<engine::material>& platform_material)
    : m_world_objects(world_objects),
    m_platforms(platforms),
    m_moving_platforms(moving_platforms),
    m_platform_material(platform_material)
{
    m_rendering_type = RenderingType::Material;
}

void MapGenerator::generate_spiral_map(int num_layers, float radius, float height_step, float platform_size)
{
    glm::vec3 last_position{};

    for (int i = 0; i < num_layers; ++i)
    {
        float angle = i * glm::radians(45.0f);
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        float y = i * height_step;

        glm::vec3 pos = { x, y, z };
        glm::vec3 scale = { platform_size, 0.2f, platform_size };

        add_static_platform(pos, scale);
        last_position = pos;

        // Every 3rd layer gets a horizontal moving platform
        if (i > 0 && i % 3 == 0)
        {
            glm::vec3 start = pos + glm::vec3(cos(angle)*6.f, 1.5f, sin(angle)*6.f);
            glm::vec3 end = start + glm::vec3(2.0f, 0.f, 0.f);
            add_moving_platform(start, end, 1.0f, scale * 0.8f);
        }

        // Every 4th layer gets a vertical moving platform
        if (i > 0 && i % 4 == 0)
        {
            glm::vec3 start = pos + glm::vec3(cos(angle)*6.f, 0.f, sin(angle)*6.f);
            glm::vec3 end = start + glm::vec3(0.f, 3.0f, 0.f);
            add_moving_platform(start, end, 0.5f, scale * 0.7f);
        }
    }

    // Place game end volume on top of spiral
    m_end_volume = std::make_shared<GameEndVolume>(last_position + glm::vec3(0.f, 0.f, 0.f), glm::vec3(3.f, 3.f, 3.f));
    //m_world_objects.push_back(m_end_volume->object());
}

void MapGenerator::add_static_platform(const glm::vec3& position, const glm::vec3& scale)
{
    auto platform = std::make_shared<Platform>(position, scale, m_platform_material, true);
    m_platforms.push_back(platform);
    m_world_objects.push_back(platform->object());
}

void MapGenerator::add_moving_platform(const glm::vec3& start, const glm::vec3& end, float speed, const glm::vec3& scale)
{
    auto moving_platform = std::make_shared<MovingPlatform>(start, end, speed, m_platform_material, scale);
    m_moving_platforms.push_back(moving_platform);
    m_world_objects.push_back(moving_platform->object());
}
