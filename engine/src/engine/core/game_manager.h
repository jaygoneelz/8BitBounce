#pragma once
#include "pch.h"
#include "engine.h"
#include "engine/entities/player.h"
#include "engine/entities/powerup.h"
#include <vector>

enum class GameState {
    Intro,
    Playing,
    GameOver
};

struct ScoreBreakdown {
    float survival_time = 0.0f;
    int powerups_collected = 0;
    int combo_streak = 0;
    int final_score = 0;
};

class GameManager {
public:
    GameManager();

    void set_player(engine::ref<Player> player) { m_player = player; }

    void update(engine::timestep time_step);
    void render();
    void on_event(engine::event& e);

    void init_gameplay();
    void cleanup_gameplay();

    void show_intro();
    void start_game();
    void trigger_game_over();
    void restart_game();

    GameState current_state() const;
    const ScoreBreakdown& score() const;

    void increment_powerup(PowerUpType type);

    float survival_time() const;

private:
    void update_score();
    void render_intro();
    void render_game();
    void render_game_over();

    GameState m_game_state = GameState::Intro;
    ScoreBreakdown m_score;
    engine::ref<Player> m_player;

    float m_survival_timer = 0.0f;
    std::vector<PowerUpType> m_collected_powerups;
    engine::ref<engine::text_manager> m_text_manager;
};

