#pragma once
#include <engine.h>
#include "platform.h"
#include "moving_platform.h"
#include "game_end_volume.h"

class MapGenerator
{
public:
    enum class RenderingType { Material, Texture };

    MapGenerator(std::vector<engine::ref<engine::game_object>>& world_objects,
        std::vector<std::shared_ptr<Platform>>& platforms,
        std::vector<std::shared_ptr<MovingPlatform>>& moving_platforms,
        const engine::ref<engine::texture_2d>& platform_texture);

    MapGenerator(std::vector<engine::ref<engine::game_object>>& world_objects,
        std::vector<std::shared_ptr<Platform>>& platforms,
        std::vector<std::shared_ptr<MovingPlatform>>& moving_platforms,
        const engine::ref<engine::material>& platform_material);

    void generate_spiral_map(int num_layers, float radius, float height_step, float platform_size);

    engine::ref<GameEndVolume> end_volume() const { return m_end_volume; }

private:
    void add_static_platform(const glm::vec3& position, const glm::vec3& scale);
    void add_moving_platform(const glm::vec3& start, const glm::vec3& end, float speed, const glm::vec3& scale);

    RenderingType m_rendering_type;
    engine::ref<engine::material> m_platform_material;
    engine::ref<engine::texture_2d> m_platform_texture;

    std::vector<engine::ref<engine::game_object>>& m_world_objects;
    std::vector<std::shared_ptr<Platform>>& m_platforms;
    std::vector<std::shared_ptr<MovingPlatform>>& m_moving_platforms;

    engine::ref<GameEndVolume> m_end_volume;
};
