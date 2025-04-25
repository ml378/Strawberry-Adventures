/**
* Author: Mahin Lalani
* Assignment: Strawberry Adventures
* Date due: 4/25/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "StrawberryMine.h"
#include "Utility.h"
#include "Map.h"
#include <cstdlib>
#include <ctime>

unsigned int strawberry_data[64];
unsigned int dirt_data[64];

StrawberryMine::~StrawberryMine()
{
    delete[]  m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void StrawberryMine::initialise()
{
    GLuint object_texture = Utility::load_texture("assets/objects.png");
    GLuint dirt_texture = Utility::load_texture("assets/dirt.png");

    std::map<int, TileUV> strawberry_uv_coords = {
        { 0, { 82.0f / 96.0f, 50.0f / 64.0f, 14.0f / 96.0f, 14.0f / 64.0f } }, // empty
        { 1, { 17.0f / 96.0f, 48.0f / 64.0f, 15.0f / 96.0f, 17.0f / 64.0f } }, // strawberry milk
        { 2, { 353.0f / 512.0f, 322.0f / 512.0f, 20.0f / 512.0f, 20.0f / 512.0f } }, // dirt
        { 3, { 464.0f / 512.0f, 379.0f / 512.0f, 20.0f / 512.0f, 20.0f / 512.0f } } // empty
    };

    std::srand(static_cast<unsigned int>(std::time(0)));
    m_success = false;
    m_failure = false;
    m_timer = 30.0f;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++) {
            m_grid[y][x] = COVERED;
        }
    }

    m_milk_x = std::rand() % WIDTH;
    m_milk_y = std::rand() % HEIGHT;
    m_grid[m_milk_y][m_milk_x] = OBJECT;

    m_pos_x = WIDTH / 2;
    m_pos_y = HEIGHT / 2;

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        strawberry_data[i] = 0;
        dirt_data[i] = 2;
    }

    strawberry_data[m_milk_y * WIDTH + m_milk_x] = 1;
    dirt_data[m_milk_y * WIDTH + m_milk_x] = 3;

    m_game_state.map = new Map(WIDTH, HEIGHT, strawberry_data, object_texture, 1.0f, 4, 1);
    m_game_state.map->set_uv_coords(strawberry_uv_coords);
    m_game_state.map->build();
    m_game_state.grass = new Map(WIDTH, HEIGHT, dirt_data, dirt_texture, 1.0f, 4, 1);
    m_game_state.grass->set_uv_coords(strawberry_uv_coords);
    m_game_state.grass->build();

    /**
     BGM and SFX
     */
    m_game_state.win_sfx = Mix_LoadWAV("assets/win.wav");
    m_game_state.loss_sfx = Mix_LoadWAV("assets/loser.wav");
}

void StrawberryMine::update(float delta_time)
{
    m_timer -= delta_time;
    if (m_timer <= 0.0f) {
        m_failure = true;
        return;
    }
    if (m_failure && !m_success) {
        m_loss = true;
    }
    if (m_success && !m_failure) {
        m_level_complete = true;
    }
    if (m_loss) {
        Mix_PlayChannel(-1, m_game_state.loss_sfx, 0);
        return;
    }
    if (m_level_complete) {
        Mix_PlayChannel(-1, m_game_state.win_sfx, 0);
        return;
    }
}


void StrawberryMine::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.grass->render(g_shader_program);
}

bool StrawberryMine::is_complete() const {
    return m_level_complete;
}

bool StrawberryMine::is_loss() const {
    return m_loss;
}

void StrawberryMine::click(float pos_x, float pos_y) {
    int tile_x = static_cast<int>(floor(pos_x + 0.3f));
    int tile_y = -static_cast<int>(floor(pos_y + 0.3f));

    if (tile_x < 0 || tile_x >= WIDTH || tile_y < 0 || tile_y >= HEIGHT) return;

    if (m_grid[tile_y][tile_x] == COVERED) {
        m_grid[tile_y][tile_x] = UNCOVERED;
        dirt_data[tile_y * WIDTH + tile_x] = 3;
        m_game_state.grass->update_tile(tile_x, tile_y, 3);
    }
    if (m_grid[tile_y][tile_x] == OBJECT) {
        m_success = true;
    }
}
