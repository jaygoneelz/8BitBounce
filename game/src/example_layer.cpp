#include "example_layer.h"
#include "platform/opengl/gl_shader.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"
#include <engine/entities/invisible_collider.h>
#include <engine/entities/shapes/cylinder.h>
#include <engine/entities/shapes/cone.h>
#include <engine/renderer/bounding_box_renderer.h>

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())


{
    // Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    engine::application::window().show_mouse_cursor();

	m_game_manager = std::make_shared<GameManager>();
	m_game_manager->show_intro();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.mp3", engine::sound_type::spatialised, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/jump.mp3", engine::sound_type::spatialised, "jump"); // Royalty free sound from pixabay.com
	m_audio_manager->load_sound("assets/audio/pickup.mp3", engine::sound_type::spatialised, "pickup"); // Royalty free sound from pixabay.com
	m_audio_manager->load_sound("assets/audio/heal.mp3", engine::sound_type::spatialised, "heal"); // Royalty free sound from pixabay.com
	m_audio_manager->load_sound("assets/audio/damage.mp3", engine::sound_type::spatialised, "damage"); // Royalty free sound from pixabay.com
	m_audio_manager->load_sound("assets/audio/pew.mp3", engine::sound_type::spatialised, "pew"); // Royalty free sound from pixabay.com
	m_audio_manager->load_sound("assets/audio/bg3.mp3", engine::sound_type::track, "music");  // Royalty free music from nosoapradio.us
	m_audio_manager->play("music");
	m_audio_manager->volume("music", 0.2f);
	//m_audio_manager->pause("music");

	// Initialize bounding box renderer
	engine::bounding_box_renderer::init();
	m_bounding_box_mesh = create_bounding_box_mesh();

	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_debug_shader = engine::renderer::shaders_library()->get("bounding_box");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	//m_light.Position = { 4.f, 10.f, -5.f };
	m_light.Color = glm::vec3(1.f,0.f,0.f);
	m_light.AmbientIntensity = 100.f;
	m_light.DiffuseIntensity = 100.f;
	m_light.Attenuation.Constant = 0.f;
	m_light.Attenuation.Exp = 0.f;
	m_light.Attenuation.Linear = 0.f;

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
		(float)engine::application::window().height()));

	m_material = engine::material::create(
		1.0f, glm::vec3(1.0f, 0.1f, 0.07f), 
		glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		1.0f
	);


	/*m_mannequin_material = engine::material::create(
		1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		1.0f
	);*/

	m_player_material = engine::material::create(
		1.0f,                               // shininess 
		glm::vec3(0.1f, 0.4f, 0.8f),        // ambient color (blue tint) 
		glm::vec3(0.1f, 0.4f, 0.8f),        // diffuse 
		glm::vec3(1.0f),         			// specular
		1.0f                                // opacity 
	);

	m_platform_material = engine::material::create(
		1.0f,
		glm::vec3(0.3f, 0.7f, 0.2f),
		glm::vec3(0.3f, 0.7f, 0.2f),
		glm::vec3(1.0f),
		1.0f
	);

	m_light_material = engine::material::create(
		1.0f,
		glm::vec3(m_light.Color),
		glm::vec3(m_light.Color),
		glm::vec3(m_light.Color),
		1.0f
	);


	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/SkyboxFront.png", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.png", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.png", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.png", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.png", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.png", true)
		});

	//// Orc warrior from https://sketchfab.com/3d-models/orc-warrior-4f8e272891a449c8935285eab8a3a2f9
	//std::vector<engine::ref<engine::texture_2d>> mannequin_textures;
	//mannequin_textures.push_back(engine::texture_2d::create("assets/textures/warrior_DefaultMaterial_BaseColor.png", false));
	//mannequin_textures.push_back(engine::texture_2d::create("assets/textures/weapon_DefaultMaterial_BaseColor.png", false));
	//engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/warrior/warrior_animation.fbx");
	//m_skinned_mesh->switch_root_movement(false);
	//m_skinned_mesh->set_textures(mannequin_textures);

	//engine::game_object_properties mannequin_props;
	//mannequin_props.animated_mesh = m_skinned_mesh;
	//mannequin_props.scale = glm::vec3(0.7f);
	//mannequin_props.position = glm::vec3(2.0f, 0.5f, -5.0f);
	//mannequin_props.textures = mannequin_textures;
	//mannequin_props.type = 0;
	//mannequin_props.bounding_shape = m_skinned_mesh->size() / 2.f * mannequin_props.scale.x;
	//m_mannequin = engine::game_object::create(mannequin_props);

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/clay.jpg", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	terrain_props.friction = 0.8f;
	m_terrain = engine::game_object::create(terrain_props);

	// Load the tree model. Create a tree object. Set its properties
	engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.0f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 4.f, 0.5f, -5.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);

	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 5.f, -5.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_ball = engine::game_object::create(sphere_props);

	engine::ref<engine::sphere> player_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties player_props;
	player_props.position = { 0.f, 2.f, 0.f };
	player_props.mass = 1.0f;
	player_props.meshes = { player_shape->mesh() };
	player_props.bounding_shape = glm::vec3(0.5f);
	player_props.restitution = 0.2f;
	player_props.friction = 1.3f;
	player_props.scale = glm::vec3(1.f);
	player_props.type = 1;
	m_player = engine::game_object::create(player_props);
	m_player_character = std::make_shared<engine::Character>(m_player);


	// platforms
	MapGenerator generator(m_game_objects, m_static_platforms, m_moving_platforms, m_platform_material);
	generator.generate_spiral_map(12, 8.0f, 1.5f, 5.0f);

	// powerups
	spawn_powerups();

	// walls
	add_boundary_walls(200.f, 200.f, 10.f, 100.f);

	// setup environment models
	const std::vector<std::string> tree_paths = {
		"assets/models/static/elm.3ds",
		"assets/models/static/QuickMapleOptimized.obj",
		//"assets/models/static/cow4.3ds" // cow for test
	};
	const std::vector<std::string> ruin_paths = {
		"assets/models/static/statue.fbx",
		//"assets/models/static/cow4.3ds" // cow for test
	};
	m_environment.set_tree_model_paths(tree_paths);
	m_environment.set_ruin_model_paths(ruin_paths);
	m_environment.generate();

	// enemies
	 spawn_enemies(50.f, 20, m_enemies);

	// include trees/ruins as collidables
	auto collidables = m_environment.get_static_collidables();
	m_game_objects.insert(m_game_objects.end(), collidables.begin(), collidables.end());

	for (auto& enemy : m_enemies)
		m_game_objects.push_back(enemy->object());


	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_ball);
	m_game_objects.push_back(m_player);
	//m_game_objects.push_back(m_tree);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);
	m_player_controller = std::make_shared<Player>(m_player, m_audio_manager, m_physics_manager, &m_3d_camera);
	m_game_manager->set_player(m_player_controller);
	m_text_manager = engine::text_manager::create();

	//m_skinned_mesh->switch_animation(6);

	m_game_end_volume = generator.end_volume();
	//m_game_end_volume = std::make_shared<GameEndVolume>(glm::vec3(3.f, 3.f, 0.f), glm::vec3(3.f, 3.f, 3.f));
}

example_layer::~example_layer() {}

void example_layer::update_third_person_camera(float pitch, float yaw)
{
	float yaw_rad = glm::radians(yaw);
	float pitch_rad = glm::radians(pitch);

	glm::vec3 cam_direction;
	cam_direction.x = sin(yaw_rad) * cos(pitch_rad);
	cam_direction.y = sin(pitch_rad);
	cam_direction.z = cos(yaw_rad) * cos(pitch_rad);

	glm::vec3 player_pos = m_player->position();
	glm::vec3 cam_offset = -cam_direction * 6.0f + glm::vec3(0.f, 2.f, 0.f); // behind + above
	glm::vec3 cam_pos = player_pos + cam_offset;

	m_3d_camera.set_view_matrix(cam_pos, player_pos);
}

void example_layer::sync_player_with_platforms()
{
	glm::vec3 player_pos = m_player->position();
	glm::vec3 player_bottom = player_pos - glm::vec3(0.0f, m_player->bounding_shape().y, 0.0f);

	for (auto& platform : m_moving_platforms)
	{
		glm::vec3 p_min = platform->object()->position() - platform->object()->bounding_shape();
		glm::vec3 p_max = platform->object()->position() + platform->object()->bounding_shape();

		// Very basic check: is player "on" the platform?
		if (player_bottom.y <= p_max.y + 0.05f && player_bottom.y >= p_max.y - 0.05f &&
			player_pos.x >= p_min.x && player_pos.x <= p_max.x &&
			player_pos.z >= p_min.z && player_pos.z <= p_max.z)
		{
			// Move player along with platform's delta
			m_player->set_position(player_pos + platform->platform_delta());
			break;
		}
	}
}

void example_layer::add_boundary_walls(float arena_width, float arena_depth, float wall_thickness, float wall_height)
{
	float half_w = arena_width / 2.f;
	float half_d = arena_depth / 2.f;

	// LEFT and RIGHT walls stretch in Z (depth)
	glm::vec3 scale_side(wall_thickness, wall_height, arena_depth);
	glm::vec3 position_left = glm::vec3(-half_w - wall_thickness / 2.f, wall_height / 2.f, 0.f);
	glm::vec3 position_right = glm::vec3(half_w + wall_thickness / 2.f, wall_height / 2.f, 0.f);

	// FRONT and BACK walls stretch in X (width)
	glm::vec3 scale_front_back(arena_width, wall_height, wall_thickness);
	glm::vec3 position_back = glm::vec3(0.f, wall_height / 2.f, -half_d - wall_thickness / 2.f);
	glm::vec3 position_front = glm::vec3(0.f, wall_height / 2.f, half_d + wall_thickness / 2.f);

	std::vector<std::pair<glm::vec3, glm::vec3>> wall_data = {
		{position_left,  scale_side},
		{position_right, scale_side},
		{position_back,  scale_front_back},
		{position_front, scale_front_back}
	};

	for (const auto& [pos, scl] : wall_data)
	{
		auto wall = std::make_shared<InvisibleCollider>(pos, scl);
		m_game_objects.push_back(wall->object());
	}
}



void example_layer::on_update(const engine::timestep& time_step)
{
	if (m_game_manager->current_state() == GameState::Intro) return;

	// local vars
	static float pitch = 15.0f;           
	static float yaw = 0.0f;              
	static float last_mouse_x = 400.f; 
	static float last_mouse_y = 300.f;
	static bool first_mouse = true;

	auto [mouse_x, mouse_y] = engine::input::mouse_position();
	if (first_mouse)
	{
		last_mouse_x = mouse_x;
		last_mouse_y = mouse_y;
		first_mouse = false;
	}

	float x_offset = mouse_x - last_mouse_x;
	float y_offset = mouse_y - last_mouse_y;

	last_mouse_x = mouse_x;
	last_mouse_y = mouse_y;

	float sensitivity = 0.1f;
	x_offset *= sensitivity;
	y_offset *= sensitivity;

	yaw -= x_offset;
	pitch -= y_offset;
	pitch = std::clamp(pitch, -80.0f, 80.0f);

	// character movement
	
	auto forward = glm::normalize(glm::vec3(m_3d_camera.front_vector().x, 0.f, m_3d_camera.front_vector().z));
	auto right = glm::normalize(glm::cross(forward, glm::vec3(0.f, 1.f, 0.f)));
	m_player_controller->on_update(time_step, forward, right);

	// 3rd person cam
	update_third_person_camera(pitch,yaw);

	// updates
	for (auto& platform : m_moving_platforms)
		platform->update(time_step);

	sync_player_with_platforms();

	// powerups
	for (auto& powerup : m_powerups)
	{
		powerup.update(time_step);

		if (!powerup.is_collected() && m_player_controller->check_collision(powerup.object()))
		{
			powerup.collect();
			m_game_manager->increment_powerup(powerup.type());

			switch (powerup.type()) {
			case PowerUpType::Heal:
				m_player_controller->heal(20);
				m_powerup_hud.push_back(
					PowerupHudItem{
						PowerUpType::Heal,
						3.f
					}
				);
				break;
			case PowerUpType::Bounce:
				m_player_controller->set_jump_powerup_multiplier(1.5f, 5.f);
				m_powerup_hud.push_back(
					PowerupHudItem{
						PowerUpType::Bounce,
						5.f
					}
				);
				break;
			default:
				break;
			}
		}
	}

	for (auto item = m_powerup_hud.begin(); item != m_powerup_hud.end(); )
	{
		item->duration -= time_step;

		if (item->duration <= 0.f)
			item = m_powerup_hud.erase(item);
		else
			++item;
	}

	static bool toggle_key_down = false;

	if (engine::input::key_pressed(engine::key_codes::KEY_B)) {
		if (!toggle_key_down) {
			m_show_bounding_boxes = !m_show_bounding_boxes;
			toggle_key_down = true;
		}
	}
	else {
		toggle_key_down = false;
	}

	for (auto& enemy : m_enemies)
		enemy->update(time_step, m_player->position(), m_player_controller);

	m_game_end_volume->on_update(time_step);
	if (m_game_end_volume->check_overlap(m_player->position())) {
		m_game_manager->trigger_game_over();
	}

	m_environment.update(time_step);

	m_3d_camera.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	//m_mannequin->animated_mesh()->on_update(time_step);

	m_audio_manager->update_with_camera(m_3d_camera);

	handle_audio();

	m_survival_time += (float)time_step.seconds();
	if (m_survival_time >= 301)
		m_game_manager->trigger_game_over();
	m_game_manager->update(time_step);
} 

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.2f, 0.3f, 0.3f, 1.0f}); 
    engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	m_light.submit(mesh_shader,1);

	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	if (m_game_manager->current_state() != GameState::Playing) {
		m_game_manager->render();
		return;
	}

	engine::renderer::begin_scene(m_3d_camera, mesh_shader);

	m_environment.render(mesh_shader);

	if (m_show_bounding_boxes) {
		for (auto& object : m_game_objects) {
			glm::vec3 bounds = object->bounding_shape();
			glm::vec3 center = object->position();

			glm::mat4 transform = glm::translate(glm::mat4(1.f), center) *
				glm::scale(glm::mat4(1.f), bounds * 2.f);

			m_debug_shader->bind();
			std::dynamic_pointer_cast<engine::gl_shader>(m_debug_shader)->set_uniform("u_color", glm::vec3(1.0f, 0.0f, 0.0f)); // Red
			engine::renderer::submit(m_debug_shader, m_bounding_box_mesh->va(), transform);
		}
	}

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(mesh_shader, m_terrain);

	glm::mat4 tree_transform(1.0f);
	tree_transform = glm::translate(tree_transform, glm::vec3(4.f, 0.5, -5.0f));
	tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	tree_transform = glm::scale(tree_transform, m_tree->scale());
	engine::renderer::submit(mesh_shader, tree_transform, m_tree);
	

	for (auto& enemy : m_enemies)
		enemy->render(mesh_shader);

	m_game_end_volume->on_render(mesh_shader);

	// light
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	m_light_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_ball);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);

	/*m_mannequin_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_mannequin);*/

	m_player_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_player);

	for (auto& platform : m_static_platforms)
		platform->render(mesh_shader);

	for (auto& platform : m_moving_platforms)
		platform->render(mesh_shader);

	// render powerups

	for (auto& powerup : m_powerups)
		powerup.render(mesh_shader);
    engine::renderer::end_scene();


	// powerup HUD
	for (int i = 0; i < m_powerup_hud.size(); i++) {
		auto& hud_item = m_powerup_hud[i];
		int seconds = static_cast<int>(hud_item.duration) % 60;
		std::stringstream hud_item_text;
		hud_item_text
			<< "Powerup! - "
			<< powerup_type_to_string(hud_item.type)
			<< " " << seconds << "s";

		m_text_manager->render_text(text_shader, hud_item_text.str(), 10.f, (float)engine::application::window().height() - 75.f - 25.f * i, 0.5f, glm::vec4(1.f, 1.0f, 1.f, 1.f));

	}
	m_game_manager->render();

	
} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event); 
        if(e.key_code() == engine::key_codes::KEY_TAB) 
        { 
            engine::render_command::toggle_wireframe();
        }
    }

	m_game_manager->on_event(event);
}

void example_layer::handle_audio()
{
	if (m_prev_sphere_y_vel < 0.1f && m_ball->velocity().y > 0.1f && m_ball->velocity().y < 0.2f) {
		m_audio_manager->play_spatialised_sound("bounce", m_3d_camera.position(), glm::vec3(m_ball->position().x, 0.f, m_ball->position().z));
	}
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}

void example_layer::spawn_powerups()
{
	m_powerups.emplace_back("assets/models/static/red_potion.obj", glm::vec3(3.f, 1.f, 2.f), PowerUpType::Heal, m_audio_manager, &m_3d_camera);
	m_powerups.emplace_back("assets/models/static/red_potion.obj", glm::vec3(6.f, 1.f, -4.f), PowerUpType::Bounce, m_audio_manager, &m_3d_camera);
}

void example_layer::spawn_enemies(float radius_from_center, int count, std::vector<std::shared_ptr<Enemy>>& enemies)
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> angle_dist(0.f, 2.f * glm::pi<float>());
	std::uniform_real_distribution<float> distance_dist(2.f, radius_from_center);  // avoid center
	std::uniform_real_distribution<float> height_dist(0.5f, 2.f); // ground level or above
	std::uniform_real_distribution<float> patrol_radius_dist(2.f, 5.f); // random patrol radius

	for (int i = 0; i < count; ++i)
	{
		float angle = angle_dist(rng);
		float distance = distance_dist(rng);

		float x = cos(angle) * distance;
		float z = sin(angle) * distance;
		float y = height_dist(rng);

		glm::vec3 spawn_pos = glm::vec3(x, y, z);
		float patrol_radius = patrol_radius_dist(rng);

		enemies.push_back(std::make_shared<Enemy>(spawn_pos, patrol_radius));
	}
}

