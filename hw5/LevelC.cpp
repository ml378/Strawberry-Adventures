/**
* Author: Mahin Lalani
* Assignment: Strawberry Adventures
* Date due: 4/25/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelC.h"
#include "Utility.h"
#include "Map.h"

#define LEVEL_WIDTH 19
#define LEVEL_HEIGHT 20

constexpr char SPRITESHEET_FILEPATH[] = "assets/char1.png",
COSTUME_FILEPATH[] = "assets/char2.png",
ENEMY_FILEPATH[] = "assets/blue_mush.png",
ICE_FILEPATH[] = "assets/ice.png",
BOSS_FILEPATH[] = "assets/BlackDragon.png";

unsigned int C_GROUND_DATA[] =
{
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
unsigned int C_GRASS_DATA[] =
{
    22, 22, 22, 22, 22, 22, 22, 22, 22, 11, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    4, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 4,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 7, 5,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 5,
    1, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 0, 0, 7, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 7, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 7, 0, 0, 0, 0, 0, 5,
    1, 0, 0, 0, 8, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
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
    GLuint map_texture_id = Utility::load_texture("assets/tilemap3.png");

    std::map<int, TileUV> ground_uv_coords = {
        { 0, { 12.0f / 112.0f, 174.0f / 320.0f, 26.0f / 112.0f, 20.0f / 320.0f } }, // ground grass block
        { 1, { 92.0f / 112.0f, 97.0f / 320.0f, 12.0f / 112.0f, 15.0f / 320.0f } }, // left edge water block
        { 2, { 38.0f / 144.0f, 144.0f / 240.0f, 19.0f / 160.0f, 16.0f / 240.0f } }, // bottom edge water block
        { 3, { 77.0f / 112.0f, 83.0f / 320.0f, 22.0f / 112.0f, 15.0f / 320.0f } }, // middle water block
        { 4, { 81.0f / 112.0f, 99.0f / 320.0f, 14.0f / 112.0f, 13.0f / 320.0f } }, // water block
        { 5, { 70.0f / 112.0f, 97.0f / 320.0f, 16.0f / 112.0f, 15.0f / 320.0f } }, // right edge water block
        { 6, { 119.0f / 144.0f, 170.0f / 240.0f, 34.0f / 160.0f, 15.0f / 240.0f } }, // corner water block
        { 7, { 15.0f / 112.0f, 34.0f / 320.0f, 17.0f / 112.0f, 14.0f / 320.0f } }, // clouds
        { 8, { 52.0f / 112.0f, 54.0f / 320.0f, 11.0f / 112.0f, 9.0f / 320.0f } }, // smoke
        { 9, { 80.0f / 144.0f, 20.0f / 240.0f, 35.0f / 160.0f, 43.0f / 240.0f } }, // first half grass block
        { 10, { 115.0f / 144.0f, 20.0f / 240.0f, 34.0f / 160.0f, 43.0f / 240.0f } }, // second half grass block
        { 11, { 16.0f / 112.0f, 64.0f / 320.0f, 16.0f / 112.0f, 16.0f / 320.0f } }, // ice hole
        { 12, { 1.0f / 144.0f, 161.0f / 240.0f, 23.0f / 160.0f, 14.0f / 240.0f } }, // left edge road
        { 13, { 15.0f / 144.0f, 161.0f / 240.0f, 16.0f / 160.0f, 14.0f / 240.0f } }, // horizontal center road
        { 14, { 49.0f / 112.0f, 209.0f / 320.0f, 14.0f / 112.0f, 23.0f / 320.0f } }, // road upwards edge
        { 15, { 49.0f / 112.0f, 232.0f / 320.0f, 14.0f / 112.0f, 23.0f / 320.0f } }, // road downwards edge
        { 16, { 49.0f / 112.0f, 222.0f / 320.0f, 14.0f / 112.0f, 16.0f / 320.0f } }, // vertical center road
        { 17, { 24.0f / 144.0f, 161.0f / 240.0f, 24.0f / 160.0f, 14.0f / 240.0f } }, // right edge road
        { 18, { 33.0f / 144.0f, 225.0f / 240.0f, 14.0f / 160.0f, 15.0f / 240.0f } }, // horizontal road bridge
        { 19, { 49.0f / 144.0f, 224.0f / 240.0f, 14.0f / 160.0f, 15.0f / 240.0f } }, // horizontal road bridge 2
        { 20, { 33.0f / 144.0f, 52.0f / 240.0f, 31.0f / 160.0f, 27.0f / 240.0f } }, // dark grass
        { 21, { 79.0f / 112.0f, 110.0f / 320.0f, 18.0f / 112.0f, 13.0f / 340.0f } }, // top water edge
        { 22, { 81.0f / 112.0f, 146.0f / 320.0f, 15.0f / 112.0f, 14.0f / 320.0f } } // empty
    };

    // Ground layer
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, C_GROUND_DATA, map_texture_id, 1.0f, 13, 11);
    m_game_state.map->set_uv_coords(ground_uv_coords);
    m_game_state.map->build();

    // Grass layer
    m_game_state.grass = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, C_GRASS_DATA, map_texture_id, 1.0f, 8, 8);
    m_game_state.grass->set_uv_coords(ground_uv_coords);
    m_game_state.grass->build();

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    GLuint costume_texture_id = Utility::load_texture(COSTUME_FILEPATH);

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        3.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        nullptr,                   // animation index sets
        0.0f,                      // animation time
        1,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        3,                         // animation row amount
        1.0f,                      // width
        1.0f,                      // height
        PLAYER
    );

    m_game_state.player->m_costume_id = costume_texture_id;

    m_game_state.player->set_position(glm::vec3(0.0f, -4.0f, 0.0f));

    // Enemies' stuff
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint boss_texture_id = Utility::load_texture(BOSS_FILEPATH);
    GLuint icicle_texture_id = Utility::load_texture(ICE_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT - 1; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, WALKER, IDLE);
        m_game_state.enemies[i].set_animation_index(0);
        m_game_state.enemies[i].set_animation_frames(1);
        m_game_state.enemies[i].set_animation_cols(8);
        m_game_state.enemies[i].set_animation_rows(7);
    }

    // enemy 1
    m_game_state.enemies[0].set_position(glm::vec3(5.0f, -3.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_game_state.enemies[0].m_w_direction = -1;
    m_game_state.enemies[0].m_walk_timer = 0.0f;
    m_game_state.enemies[0].m_w_direction_timer = 0.0f;
    m_game_state.enemies[0].m_w_direction_interval = 3.0f;
    // enemy 2
    m_game_state.enemies[1].set_position(glm::vec3(2.0f, 0.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_game_state.enemies[1].m_w_direction = -1;
    m_game_state.enemies[1].m_walk_timer = 0.0f;
    m_game_state.enemies[1].m_w_direction_timer = 0.0f;
    m_game_state.enemies[1].m_w_direction_interval = 3.0f;
    // enemy 3
    m_game_state.enemies[3].set_position(glm::vec3(5.0f, 3.0f, 0.0f));
    m_game_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[3].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_game_state.enemies[3].m_w_direction = -1;
    m_game_state.enemies[3].m_walk_timer = 0.0f;
    m_game_state.enemies[3].m_w_direction_timer = 0.0f;
    m_game_state.enemies[3].m_w_direction_interval = 3.0f;

    // boss
    m_game_state.enemies[4] = Entity(boss_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, FLYER, IDLE);
    m_game_state.enemies[4].set_animation_index(0);
    m_game_state.enemies[4].set_animation_frames(1);
    m_game_state.enemies[4].set_animation_cols(4);
    m_game_state.enemies[4].set_animation_rows(8);
    m_game_state.enemies[4].set_scale(glm::vec3(2.0f, 2.0f, 1.0f));

    m_game_state.enemies[4].set_position(glm::vec3(0.0f, 7.0f, 0.0f));
    m_game_state.enemies[4].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[4].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_game_state.enemies[4].m_direction = -1;
    m_game_state.enemies[4].m_fly_timer = 0.0f;
    m_game_state.enemies[4].m_direction_timer = 0.0f;
    m_game_state.enemies[4].m_direction_interval = 5.0f;

    // icicles
    m_game_state.enemies[5] = Entity(icicle_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, PROJECTILE, IDLE);
    m_game_state.enemies[5].set_animation_index(0);
    m_game_state.enemies[5].set_animation_frames(0);
    m_game_state.enemies[5].set_animation_cols(1);
    m_game_state.enemies[5].set_animation_rows(1);
    m_game_state.enemies[5].set_scale(glm::vec3(5.0f, 5.0f, 1.0f));

    m_game_state.enemies[5].set_position(glm::vec3(-2.0f, -2.5f, 0.0f));
    m_game_state.enemies[5].set_start_position(glm::vec3(-2.0f, -2.5f, 0.0f));
    m_game_state.enemies[5].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[5].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_game_state.enemies[5].m_projectile_timer = 0.0f;
    m_game_state.enemies[5].m_motion_timer = 0.0f;
    m_game_state.enemies[5].m_motion_interval = 3.0f;

    m_game_state.enemies[6] = Entity(icicle_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, PROJECTILE, IDLE);
    m_game_state.enemies[6].set_animation_index(0);
    m_game_state.enemies[6].set_animation_frames(0);
    m_game_state.enemies[6].set_animation_cols(1);
    m_game_state.enemies[6].set_animation_rows(1);
    m_game_state.enemies[6].set_scale(glm::vec3(5.0f, 5.0f, 1.0f));

    m_game_state.enemies[6].set_position(glm::vec3(-4.0f, 0.0f, 0.0f));
    m_game_state.enemies[6].set_start_position(glm::vec3(-4.0f, 0.0f, 0.0f));
    m_game_state.enemies[6].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[6].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_game_state.enemies[6].m_projectile_timer = 0.0f;
    m_game_state.enemies[6].m_motion_timer = 0.0f;
    m_game_state.enemies[6].m_motion_interval = 5.0f;

    m_game_state.enemies[7] = Entity(icicle_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, PROJECTILE, IDLE);
    m_game_state.enemies[7].set_animation_index(0);
    m_game_state.enemies[7].set_animation_frames(0);
    m_game_state.enemies[7].set_animation_cols(1);
    m_game_state.enemies[7].set_animation_rows(1);
    m_game_state.enemies[7].set_scale(glm::vec3(5.0f, 5.0f, 1.0f));

    m_game_state.enemies[7].set_position(glm::vec3(-4.0f, 5.0f, 0.0f));
    m_game_state.enemies[7].set_start_position(glm::vec3(-2.0f, 3.0f, 0.0f));
    m_game_state.enemies[7].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[7].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    m_game_state.enemies[7].m_projectile_timer = 0.0f;
    m_game_state.enemies[7].m_motion_timer = 0.0f;
    m_game_state.enemies[7].m_motion_interval = 5.0f;

    /**
     BGM and SFX
     */
    m_game_state.death_sfx = Mix_LoadWAV("assets/death.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/win.wav");
    m_game_state.loss_sfx = Mix_LoadWAV("assets/loser.wav");

}

void LevelC::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, m_game_state.grass);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, m_game_state.grass);
    }
    if (m_game_state.player->get_position().y >= 9.4f && m_game_state.player->get_position().x >= -0.1f)
    {
        m_win = true;
    }
    if (m_game_state.lives <= 0 && !m_loss) {
        m_loss = true;
    }
    for (int i = 0; i < ENEMY_COUNT - 1; i++) {
        if (m_game_state.player->check_collision(&m_game_state.enemies[i]))
        {
            m_game_state.lives--;
            m_life_lost = true;
            m_life_loss_timer = 0.3f;
            if (m_game_state.lives > 0 && !m_loss) {
                Mix_PlayChannel(-1, m_game_state.death_sfx, 0);
                m_game_state.player->set_position(glm::vec3(0.0f, -4.0f, 0.0f));
            }
        }
        if (m_game_state.player->check_collision(&m_game_state.enemies[4])) {
            m_game_state.lives--;
            if (m_game_state.lives > 0 && !m_loss) {
                Mix_PlayChannel(-1, m_game_state.death_sfx, 0);
                m_game_state.player->set_position(glm::vec3(0.0f, -4.0f, 0.0f));
            }
        }
    }
    if (m_loss) {
        Mix_PlayChannel(-1, m_game_state.loss_sfx, 0);
        return;
    }
    if (m_win) {
        Mix_PlayChannel(-1, m_game_state.win_sfx, 0);
        return;
    }
    if (m_life_lost && !m_loss) {
        m_life_loss_timer -= delta_time;
        if (m_life_loss_timer <= 0.0f) {
            m_life_lost = false;
        }
    }
}

void LevelC::render(ShaderProgram* g_shader_program)
{
    g_shader_program->set_loss_effect(m_life_lost);
    m_game_state.map->render(g_shader_program);
    m_game_state.grass->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++)
        m_game_state.enemies[i].render(g_shader_program);
}

bool LevelC::is_loss() const {
    return m_loss;
}

bool LevelC::is_won() const {
    return m_win;
}