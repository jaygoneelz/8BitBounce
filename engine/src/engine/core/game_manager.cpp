#include "pch.h"
#include "game_manager.h"
#include <sstream>
#include <iomanip>
#include <engine.h>
#include <engine/events/key_event.h>
#include <gl/GL.h>

GameManager::GameManager()
    : m_game_state(GameState::Intro), m_survival_timer(0.0f), m_score() {
    m_text_manager = engine::text_manager::create();
}

void GameManager::update(engine::timestep time_step) {
    if (m_game_state == GameState::Playing) {
        m_survival_timer += (float)time_step.seconds();
        m_score.survival_time = m_survival_timer;

        if (m_player && m_player->health() <= 0) {
            trigger_game_over();
        }

        update_score();
    }
}

void GameManager::render() {
    switch (m_game_state) {
    case GameState::Intro:      render_intro(); break;
    case GameState::Playing:    render_game(); break;
    case GameState::GameOver:   render_game_over(); break;
    }
}

void GameManager::on_event(engine::event& e) {
    if (e.event_type() == engine::event_type_e::key_pressed) {
        auto& key_event = dynamic_cast<engine::key_pressed_event&>(e);
        if (m_game_state == GameState::Intro && key_event.key_code() == engine::key_codes::KEY_SPACE) {
            start_game();
        }
        else if (m_game_state == GameState::GameOver) {
            if (key_event.key_code() == engine::key_codes::KEY_R) {
                restart_game();
            }
            else if (key_event.key_code() == engine::key_codes::KEY_ESCAPE) {
                show_intro();
            }
        }
    }
}

void GameManager::init_gameplay() {
    m_survival_timer = 0.0f;
    m_collected_powerups.clear();
    m_score = ScoreBreakdown();
    m_game_state = GameState::Playing;
}

void GameManager::cleanup_gameplay() {
    // Clean up gameplay state if needed
}

void GameManager::show_intro() {
    cleanup_gameplay();
    m_game_state = GameState::Intro;
}

void GameManager::start_game() {
    init_gameplay();
}

void GameManager::trigger_game_over() {
    update_score();
    m_game_state = GameState::GameOver;
}

void GameManager::restart_game() {
    init_gameplay();
}

GameState GameManager::current_state() const {
    return m_game_state;
}

const ScoreBreakdown& GameManager::score() const {
    return m_score;
}

float GameManager::survival_time() const {
    return m_survival_timer;
}

void GameManager::increment_powerup(PowerUpType type) {
    m_collected_powerups.push_back(type);
    m_score.powerups_collected++;
}

void GameManager::update_score() {
    m_score.final_score = (int)(m_score.survival_time * 10 + m_score.powerups_collected * 50 + m_score.combo_streak * 100);
}

void GameManager::render_intro() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /*glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);*/


    const auto shader = engine::renderer::shaders_library()->get("text_2D");
    const auto width = engine::application::window().width();
    const auto height = engine::application::window().height();

    float center_x = width * 0.25f;

    // Title
    m_text_manager->render_text(shader, "8-bit Bounce", center_x, height * 0.7f, 1.5f, glm::vec4(1.f, 0.85f, 0.25f, 1.f));

    // Controls
    m_text_manager->render_text(shader, "Controls:", center_x, height * 0.6f, 1.0f, glm::vec4(0.8f, 1.f, 0.8f, 1.f));
    m_text_manager->render_text(shader, "  - W, A, S, D to move", center_x, height * 0.55f, 0.9f, glm::vec4(0.7f, 0.85f, 1.f, 1.f));
    m_text_manager->render_text(shader, "  - Mouse to look around", center_x, height * 0.50f, 0.9f, glm::vec4(0.7f, 0.85f, 1.f, 1.f));
    m_text_manager->render_text(shader, "  - Space to jump", center_x, height * 0.45f, 0.9f, glm::vec4(0.7f, 0.85f, 1.f, 1.f));
    // Objective
    m_text_manager->render_text(shader, "Objective:", center_x, height * 0.37f, 1.0f, glm::vec4(0.8f, 1.f, 0.8f, 1.f));
    m_text_manager->render_text(shader, "  Climb to the top of the Spiral", center_x, height * 0.3f, 0.9f, glm::vec4(0.7f, 0.85f, 1.f, 1.f));
    m_text_manager->render_text(shader, "  or survive for 5 minutes", center_x, height * 0.23f, 0.9f, glm::vec4(0.7f, 0.85f, 1.f, 1.f));
    m_text_manager->render_text(shader, "  Avoid enemies (Red Cubes)", center_x, height * 0.18f, 0.9f, glm::vec4(0.7f, 0.85f, 1.f, 1.f));

    // Prompt
    m_text_manager->render_text(shader, "< Press Space to start >", center_x, height * 0.09f, 1.0f, glm::vec4(1.f, 0.7f, 0.7f, 1.f));
}


void GameManager::render_game() {
    /*glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);*/

    const auto shader = engine::renderer::shaders_library()->get("text_2D");
    auto width = engine::application::window().width();
    auto height = engine::application::window().height();

    int max_health = m_player ? m_player->max_health() : 100;
    int current_health = m_player ? m_player->health() : 100;

    glm::vec4 health_color = glm::vec4(0.f, 1.f, 0.f, 1.f);
    if (current_health < (max_health * 70) / 100)
        health_color = glm::vec4(1.f, 1.f, 0.f, 1.f);
    if (current_health < (max_health * 40) / 100)
        health_color = glm::vec4(1.f, 0.5f, 0.f, 1.f);
    if (current_health < (max_health * 20) / 100)
        health_color = glm::vec4(1.f, 0.f, 0.f, 1.f);

    std::stringstream health_text;
    health_text << "Health: " << current_health << "/" << max_health;
    m_text_manager->render_text(shader, health_text.str(), width * 0.02f, height * 0.95f, 0.75f, health_color);

    std::stringstream time_text;
    int minutes = (int)m_survival_timer / 60;
    int seconds = (int)m_survival_timer % 60;
    time_text << "Time Survived: " << minutes << "m " << seconds << "s";
    m_text_manager->render_text(shader, time_text.str(), width * 0.7f, height * 0.95f, 0.75f, glm::vec4(1.f));

    std::stringstream score_text;
    score_text << "Score: " << m_score.final_score;
    m_text_manager->render_text(shader, score_text.str(), width * 0.45f, height * 0.95f, 0.75f, glm::vec4(1.f));
}

void GameManager::render_game_over() {
    /*glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);*/

    const auto shader = engine::renderer::shaders_library()->get("text_2D");
    auto width = engine::application::window().width();
    auto height = engine::application::window().height();

    glClearColor(0.05f, 0.05f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_text_manager->render_text(shader, "Game Over", width * 0.3f, height * 0.6f, 1.2f, glm::vec4(1.f));

    std::stringstream score_text;
    score_text << "Final Score: " << m_score.final_score;
    m_text_manager->render_text(shader, score_text.str(), width * 0.3f, height * 0.5f, 1.f, glm::vec4(1.f));

    int minutes = (int)m_score.survival_time / 60;
    int seconds = (int)m_score.survival_time % 60;
    std::stringstream detail_text;
    detail_text << "Survival: " << minutes << "m"<< seconds << "s  Powerups: " << m_score.powerups_collected << "  Combos: " << m_score.combo_streak;
    m_text_manager->render_text(shader, detail_text.str(), width * 0.3f, height * 0.45f, 0.8f, glm::vec4(0.8f));

    //m_text_manager->render_text(shader, "Press R to Restart", width * 0.3f, height * 0.3f, 0.8f, glm::vec4(0.6f));
    m_text_manager->render_text(shader, "Press ESC to Exit", width * 0.3f, height * 0.35f, 0.8f, glm::vec4(0.6f));
}
