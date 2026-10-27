#pragma once
#include "pch.h"
#include <engine.h>
#include "engine/core/game_manager.h"
#include "engine/entities/Character.h"
#include "engine/entities/platform.h"
#include "engine/entities/moving_platform.h"
#include "engine/entities/map_generator.h"
#include <engine/entities/player.h>
#include <engine/entities/powerup.h>
#include <engine/entities/shapes/quad.h>
#include <engine/helpers/environment_generator.h>
#include <engine/entities/enemy.h>
#include <engine/entities/game_end_volume.h>


typedef struct {
	PowerUpType type;
	float duration;
} PowerupHudItem;

class example_layer : public engine::layer
{
public:
    example_layer();
	~example_layer();

	void sync_player_with_platforms();
	void add_boundary_walls(float arena_width, float arena_height, float wall_thickness, float wall_height);
	void update_third_person_camera(float pitch, float yaw);

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

private:
	void handle_audio();

	void spawn_powerups();

	void spawn_enemies(float radius_from_center, int count, std::vector<std::shared_ptr<Enemy>>& enemies);

	std::string powerup_type_to_string(PowerUpType type) {
		switch (type) {
		case PowerUpType::Bounce: return "Bounce";
		case PowerUpType::Heal: return "Heal";
		default: return "Unknown";
		}
	}

	engine::ref<engine::mesh> create_bounding_box_mesh()
	{
		std::vector<glm::vec3> positions = {
			{-0.5f, -0.5f, -0.5f}, // 0
			{ 0.5f, -0.5f, -0.5f}, // 1
			{ 0.5f,  0.5f, -0.5f}, // 2
			{-0.5f,  0.5f, -0.5f}, // 3
			{-0.5f, -0.5f,  0.5f}, // 4
			{ 0.5f, -0.5f,  0.5f}, // 5
			{ 0.5f,  0.5f,  0.5f}, // 6
			{-0.5f,  0.5f,  0.5f}, // 7
		};

		std::vector<uint32_t> indices = {
			0, 1, 1, 2, 2, 3, 3, 0, // back face edges
			4, 5, 5, 6, 6, 7, 7, 4, // front face edges
			0, 4, 1, 5, 2, 6, 3, 7  // side edges
		};

		std::vector<engine::mesh::vertex> vertices;
		for (const auto& pos : positions)
			vertices.push_back({ pos, glm::vec3(0.f), glm::vec2(0.f) });

		return engine::mesh::create(vertices, indices);
	}

	// world
	engine::ref<engine::skybox>							m_skybox{};
	engine::ref<engine::game_object>					m_terrain{};
	engine::ref<engine::game_object>					m_cow{};
	engine::ref<engine::game_object>					m_tree{};
	engine::ref<engine::game_object>					m_ball{};
	engine::ref<engine::game_object>					m_mannequin{};
	engine::ref<engine::mesh>							m_bounding_box_mesh;
	std::vector<engine::ref<Platform>>					m_static_platforms;
	std::vector<engine::ref<MovingPlatform>>			m_moving_platforms;

	// player
	engine::ref<engine::game_object>					m_player{};
	engine::ref<engine::Character>						m_player_character{};
	engine::ref<Player>									m_player_controller;
	bool												m_can_jump = true;
	float												m_prev_player_y_vel = 0.0f;
	bool												m_jump_key_held = false;
	float												m_survival_time = 0.0f;

	// enemies
	std::vector<std::shared_ptr<Enemy>>					m_enemies;

	std::shared_ptr<GameEndVolume> m_game_end_volume;


	engine::ref<engine::material>						m_material{};
	engine::ref<engine::material>						m_mannequin_material{};
	engine::ref<engine::material>						m_player_material{};
	engine::ref<engine::material>						m_platform_material;
	engine::ref<engine::material>						m_light_material;

	GameState											m_game_state = GameState::Intro;

	engine::ref<engine::texture_2d>						m_intro_texture;
	engine::ref<engine::texture_2d>						m_outro_texture;
	engine::ref<engine::quad>							m_screen_quad;

	engine::DirectionalLight							m_directionalLight;
	engine::PointLight									m_light;

	std::vector<engine::ref<engine::game_object>>		m_game_objects{};
	EnvironmentGenerator								m_environment = EnvironmentGenerator(100.f, 35.f);

	float												m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::bullet_manager>					m_physics_manager{};
	engine::ref<engine::audio_manager>					m_audio_manager{};
	engine::ref<engine::text_manager>					m_text_manager{};
	engine::ref<GameManager>							m_game_manager{};

	std::vector<PowerUp>								m_powerups;
	std::vector<PowerUpType>							m_active_powerups;
	std::vector<PowerupHudItem>							m_powerup_hud;

    engine::orthographic_camera							m_2d_camera; 
    engine::perspective_camera							m_3d_camera;
	engine::ref<engine::shader>							m_debug_shader;
	bool												m_show_bounding_boxes = false;
};
