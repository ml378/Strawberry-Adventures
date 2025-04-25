/**
* Author: Mahin Lalani
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/Attack_KG_1.png",
ENEMY_FILEPATH[] = "assets/Necromancer.png";

unsigned int LEVEL_C_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 18, 18, 18, 18, 18,
    0, 0, 0, 0, 0, 0, 18, 19, 0, 105, 105, 105, 105, 105, 105, 105,
    18, 19, 0, 0, 0, 0, 0, 0, 0, 105, 105, 105, 105, 105, 105, 105,
    0, 0, 0, 0, 0, 19, 20, 0, 0, 105, 105, 105, 105, 105, 105, 105,
    18, 19, 19, 20, 0, 0, 0, 0, 0, 105, 105, 105, 105, 105, 105, 105
};

LevelC::~LevelC()
{
    delete[]  m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelC::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/tileset4.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_C_DATA, map_texture_id, 1.0f, 13, 11);

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        nullptr,  // animation index sets
        0.0f,                      // animation time
        1,                         // animation frame amount
        0,                         // current animation index
        6,                         // animation column amount
        1,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(1.5f, -5.5f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(4.0f);

    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, FLYER, IDLE);
        m_game_state.enemies[i].set_animation_index(0);
        m_game_state.enemies[i].set_animation_frames(1);
        m_game_state.enemies[i].set_animation_cols(17);
        m_game_state.enemies[i].set_animation_rows(7);
        m_game_state.enemies[i].set_scale(glm::vec3(2.65f, 2.65f, 1.0f));
    }


    m_game_state.enemies[0].set_position(glm::vec3(8.1f, -2.65f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_game_state.enemies[0].m_direction = -1;
    m_game_state.enemies[0].m_fly_timer = 0.0f;
    m_game_state.enemies[0].m_direction_timer = 0.0f;
    m_game_state.enemies[0].m_direction_interval = 1.5f;


    /**
     BGM and SFX
     */
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
    m_game_state.death_sfx = Mix_LoadWAV("assets/death.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/win.wav");
    m_game_state.loss_sfx = Mix_LoadWAV("assets/loser.wav");
}

void LevelC::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
    if (m_game_state.player->get_position().x >= 14.0f && !m_win) {
        m_win = true;
        m_win_timer = 0.0f;
    }

    if (m_game_state.lives <= 0 && !m_loss) {
        m_loss = true;
        m_lose_timer = 0.0f;
    }
    if (m_game_state.player->check_collision(&m_game_state.enemies[0]))
    {
        m_game_state.lives--;
        if (m_game_state.lives > 0 && !m_loss) {
            Mix_PlayChannel(-1, m_game_state.death_sfx, 0);
            m_game_state.player->set_position(glm::vec3(1.5f, -5.5f, 0.0f));
        }
    }
    if (m_win) {
        Mix_PlayChannel(-1, m_game_state.win_sfx, 0);
        m_win_timer += delta_time;
        if (m_win_timer >= 4.0f) {
            SDL_Quit();
            exit(0);
        }
        return;
    }
    if (m_loss) {
        Mix_PlayChannel(-1, m_game_state.loss_sfx, 0);
        m_lose_timer += delta_time;
        if (m_lose_timer >= 2.0f) {
            SDL_Quit();
            exit(0);
        }
        return;
    }
}


void LevelC::render(ShaderProgram* g_shader_program)
{
    if (m_loss) {
        Utility::draw_text(g_shader_program, Utility::load_texture("assets/font1.png"), "You Lose", 0.5f, -0.23f, glm::vec3(4.0f, -3.5f, 0.0f));
        return;
    }
    if (m_win) {
        Utility::draw_text(g_shader_program, Utility::load_texture("assets/font1.png"), "You Win", 0.5f, -0.23f, glm::vec3(11.0f, -3.5f, 0.0f));
        return;
    }
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++)
        m_game_state.enemies[i].render(g_shader_program);
}

bool LevelC::is_complete() const{
    return m_level_complete;
}
