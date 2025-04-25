/**
* Author: Mahin Lalani
* Assignment: Strawberry Adventures
* Date due: 4/25/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Menu.h"
#include "Loss.h"
#include "Win.h"
#include "StrawberryMine.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 700,
WINDOW_HEIGHT = 560;

constexpr float MAX_RGB = 255.0f;
constexpr float BG_RED = 25.0f / MAX_RGB;
constexpr float BG_GREEN = 48.0f / MAX_RGB;
constexpr float BG_BLUE = 40.0f / MAX_RGB;
constexpr float BG_OPACITY = 1.0f;


constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_exercise.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene* g_current_scene;
Scene* g_menu;
Scene* g_loss;
Scene* g_win;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
StrawberryMine* g_strawberrymine;
int current_level = 0;
int g_player_lives = 5;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix, g_level_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;
float close_time = 0.0f;

void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Strawberry Adventures",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);

    g_projection_matrix = glm::ortho(-9.375f, 9.375f, -7.5f, 7.5f, -1.0f, 1.0f);
    float zoomFactor = 0.5f;
    float left = -9.375f * zoomFactor;
    float right = 9.375f * zoomFactor;
    float bottom = -7.5f * zoomFactor;
    float top = 7.5f * zoomFactor;

    g_level_projection_matrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(0.47f, 0.95f, 0.82f, 1.0f);

    g_menu = create_menu_scene();
    switch_to_scene(g_menu);
    g_loss = create_loss_scene();
    g_win = create_win_scene();

    // ————— LEVEL SETUP ————— //
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    g_strawberrymine = new StrawberryMine();

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Music
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Music* bgm = Mix_LoadMUS("assets/adventure.mp3");
    Mix_PlayMusic(bgm, -1);
}

void process_input()
{
    if (g_current_scene->get_state().player != nullptr) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_app_status = TERMINATED;
                break;
            case SDLK_RETURN:
                switch_to_scene(g_level_a);
                break;
            case SDLK_1:
                switch_to_scene(g_level_a);
                break;
            case SDLK_2:
                switch_to_scene(g_level_b);
                break;
            case SDLK_3:
                switch_to_scene(g_level_c);
                glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
                break;
            case SDLK_4:
                switch_to_scene(g_strawberrymine);
                glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
                g_projection_matrix = glm::ortho(0.0f, 8.0f, -8.0f, 0.0f, -1.0f, 1.0f);
                g_view_matrix = glm::mat4(1.0f);
            default:
                break;
            }

        default:
            break;
        }
    }

    // ————— KEY HOLD ————— //
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (g_current_scene->get_state().player != nullptr) {
        if (key_state[SDL_SCANCODE_LEFT])   g_current_scene->get_state().player->move_left();
        if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
        if (key_state[SDL_SCANCODE_UP])     g_current_scene->get_state().player->move_up();
        if (key_state[SDL_SCANCODE_DOWN])   g_current_scene->get_state().player->move_down();

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        glm::vec2 mouse_pos = Utility::pos_coords(mouse_x, mouse_y, WINDOW_WIDTH, WINDOW_HEIGHT, g_view_matrix, g_projection_matrix);

        g_strawberrymine->click(mouse_pos.x, mouse_pos.y);
    }
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;


    // ————— PLAYER CAMERA ————— //

    if (g_current_scene->get_state().player != nullptr) {
        glm::vec3 player_pos = g_current_scene->get_state().player->get_position();

        float map_top = (g_current_scene->get_state().map->get_height() * g_current_scene->get_state().map->get_tile_size()) / 2.0f;
        float map_bottom = -map_top;

        float view_height = 7.5f;

        float min_camera_y = map_bottom + view_height;
        float max_camera_y = map_top - view_height;

        float camera_x = player_pos.x;
        float camera_y = player_pos.y;

        g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-camera_x, -camera_y, 0.0f));
    }
    else {
        g_view_matrix = glm::mat4(1.0f);
    }


    // ————— LEVEL PROGRESSION ————— //
    if (g_current_scene == g_level_a && g_level_a->is_complete()) {
        g_player_lives = g_current_scene->get_state().lives;
        g_current_scene = g_level_b;
        g_current_scene->get_state().lives = g_player_lives;
        g_current_scene->initialise();
    }
    /*if (g_current_scene == g_level_a && g_level_a->is_mining_tile()) {
        switch_to_scene(g_strawberrymine);
        g_projection_matrix = glm::ortho(0.0f, 8.0f, -8.0f, 0.0f, -1.0f, 1.0f);
        g_view_matrix = glm::mat4(1.0f);
    }*/
    /*if (g_current_scene == g_strawberrymine && g_strawberrymine->is_complete()) {
        switch_to_scene(g_level_b);
    }*/
    if (g_current_scene == g_level_a && g_level_a->is_loss()) {
        switch_to_scene(g_loss);
    }
    if (g_current_scene == g_level_b && g_level_b->is_loss()) {
        switch_to_scene(g_loss);
    }
    if (g_current_scene == g_level_b && g_level_b->is_complete()) {
        g_player_lives = g_current_scene->get_state().lives;
        g_current_scene = g_level_c;
        g_current_scene->get_state().lives = g_player_lives;
        g_current_scene->initialise();
        glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
    }
    if (g_current_scene == g_level_c && g_level_c->is_loss()) {
        switch_to_scene(g_loss);
    }
    if (g_current_scene == g_level_c && g_level_c->is_won()) {
        switch_to_scene(g_win);
    }
}

void render()
{
    if (g_current_scene == g_menu) {
        g_shader_program.set_projection_matrix(g_projection_matrix);
    }
    else {
        g_shader_program.set_projection_matrix(g_level_projection_matrix);
    }

    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}