/**
* Author: Mahin Lalani
* Assignment: Lunar Lander
* Date due: 2025-3-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char  SPRITESHEET_FILEPATH[] = "assets/star.png",
PLATFORM_FILEPATH[] = "assets/platform.png",
PLATFORM2_FILEPATH[] = "assets/peak.png",
SUCCESS_FILEPATH[] = "assets/success.png",
FAILURE_FILEPATH[] = "assets/failure.png";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
constexpr float ACC_OF_GRAVITY = -3.0f;
constexpr int   PLATFORM_COUNT = 3;

// ————— STRUCTS AND ENUMS —————//
enum AppStatus { RUNNING, TERMINATED };

struct GameState
{
    Entity* player;
    Entity* platforms;
};

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_time_accumulator = 0.0f;

bool success = false;
bool failure = false;
Entity* mission_failed = nullptr;
Entity* mission_succeeded = nullptr;
float close_time = 0.0f;

// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath);

void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components,
        STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
        GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Lunar Lander",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (context == nullptr)
    {
        LOG("ERROR: Could not create OpenGL context.\n");
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // ————— PLAYER ————— //
    GLuint player_texture_id = load_texture(SPRITESHEET_FILEPATH);

    g_game_state.player = new Entity(
        player_texture_id,         // texture id
        1.0f,                      // speed
        nullptr,
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4                          // animation row amount
    );
    
    g_game_state.player->set_position(glm::vec3(0.0f, 3.5f, 0.0f));
    g_game_state.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.1, 0.0f));
    g_game_state.player->set_scale(glm::vec3(0.9f, 0.9f, 0.9f));

    // ————— PLATFORM ————— //
    g_game_state.platforms = new Entity[PLATFORM_COUNT];

    GLuint platform = load_texture(PLATFORM_FILEPATH);  
    GLuint platform2 = load_texture(PLATFORM2_FILEPATH); 

    // Main center platform
    g_game_state.platforms[0].set_texture_id(load_texture(PLATFORM_FILEPATH));
    g_game_state.platforms[0].set_position(glm::vec3(0.0f, -3.5f, 0.0f));
    g_game_state.platforms[0].set_scale(glm::vec3(5.0f, 3.0f, 1.0f));
    g_game_state.platforms[0].update(0.0f, nullptr, 0);

    // Left side platform
    g_game_state.platforms[1].set_texture_id(load_texture(PLATFORM2_FILEPATH));
    g_game_state.platforms[1].set_position(glm::vec3(-4.5f, -3.5f, 1.0f)); 
    g_game_state.platforms[1].set_scale(glm::vec3(4.0f, 4.0f, 1.0f)); 
    g_game_state.platforms[1].update(0.0f, nullptr, 0);

    // Right side platform
    g_game_state.platforms[2].set_texture_id(load_texture(PLATFORM2_FILEPATH));
    g_game_state.platforms[2].set_position(glm::vec3(4.5f, -3.5f, 1.0f)); 
    g_game_state.platforms[2].set_scale(glm::vec3(4.0f, 3.0f, 1.0f)); 
    g_game_state.platforms[2].update(0.0f, nullptr, 0);
    
    // Success and Failure //
    GLuint mission_success = load_texture(SUCCESS_FILEPATH);
    GLuint mission_failure = load_texture(FAILURE_FILEPATH);

    mission_failed = new Entity();
    mission_failed->set_texture_id(mission_failure);
    mission_failed->set_position(glm::vec3(0.0f, 1.0f, 0.0f));
    mission_failed->set_scale(glm::vec3(4.5f, 3.0f, 1.0f));
    
    mission_succeeded = new Entity();
    mission_succeeded->set_texture_id(mission_success);
    mission_succeeded->set_position(glm::vec3(0.0f, 1.0f, 0.0f));
    mission_succeeded->set_scale(glm::vec3(5.0f, 3.0f, 1.0f));

    // ————— GENERAL ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_game_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
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

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    glm::vec3 acceleration = g_game_state.player->get_acceleration();

    // Upwards Thrust
    if (key_state[SDL_SCANCODE_SPACE] && g_game_state.player->get_fuel() > 0)
    {
        acceleration.y += 0.01f; 
        g_game_state.player->consume_fuel(1.0f);
    }

    // Left and Right Acceleration
    if (key_state[SDL_SCANCODE_LEFT] && g_game_state.player->get_fuel() > 0)
    {
        acceleration.x -= 0.5f; 
        g_game_state.player->consume_fuel(1.0f);
    }
    if (key_state[SDL_SCANCODE_RIGHT] && g_game_state.player->get_fuel() > 0)
    {
        acceleration.x += 0.5f;
        g_game_state.player->consume_fuel(1.0f);
    }

    g_game_state.player->set_acceleration(acceleration);
}

void update()
{
    // ————— DELTA TIME ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    // ————— FIXED TIMESTEP ————— //
    // STEP 1: Keep track of how much time has passed since last step
    delta_time += g_time_accumulator;

    // STEP 2: Accumulate the ammount of time passed while we're under our fixed timestep
    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    // STEP 3: Once we exceed our fixed timestep, apply that elapsed time into the 
    //         objects' update function invocation
    while (delta_time >= FIXED_TIMESTEP)
    {
        // Notice that we're using FIXED_TIMESTEP as our delta time
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.platforms,
            PLATFORM_COUNT);
        delta_time -= FIXED_TIMESTEP;
    }

    g_time_accumulator = delta_time;

    if (g_game_state.player->get_position().x <= -4.5 || g_game_state.player->get_position().x >= 4.5) {
        failure = true;
        close_time = SDL_GetTicks();
        g_game_state.player->set_acceleration(glm::vec3(0.0f));
        g_game_state.player->set_velocity(glm::vec3(0.0f));
        g_game_state.player->set_movement(glm::vec3(0.0f));
        if (g_game_state.player->get_position().x <= -4.5)
        {
            g_game_state.player->set_position(glm::vec3(-4.5 + 0.2f, g_game_state.player->get_position().y, 0.0f));
        }
        else if (g_game_state.player->get_position().x >= 4.5)
        {
            g_game_state.player->set_position(glm::vec3(4.5 - 0.2f, g_game_state.player->get_position().y, 0.0f));
        }
    }
    // Collision with Center Platform
    Entity* center_platform = &g_game_state.platforms[0]; 
    if (g_game_state.player->check_collision(center_platform))
    {
        failure = true;
        g_game_state.player->set_acceleration(glm::vec3(0.0f));
        g_game_state.player->set_velocity(glm::vec3(0.0f));
        g_game_state.player->set_movement(glm::vec3(0.0f));
        close_time = SDL_GetTicks();
        float top = center_platform->get_position().y + (center_platform->get_scale().y / 2);
        float height = g_game_state.player->get_scale().y / 2;
        g_game_state.player->set_position(glm::vec3(g_game_state.player->get_position().x, top + height, 0.0f));
    }
    // Collision with Side Platforms
    Entity* left_side = &g_game_state.platforms[1];
    if (g_game_state.player->check_collision(left_side))
    {
        success = true;
        g_game_state.player->set_acceleration(glm::vec3(0.0f));
        g_game_state.player->set_velocity(glm::vec3(0.0f));
        g_game_state.player->set_movement(glm::vec3(0.0f));
        close_time = SDL_GetTicks();
        float top = left_side->get_position().y + (left_side->get_scale().y / 2);
        float height = g_game_state.player->get_scale().y / 2;
        g_game_state.player->set_position(glm::vec3(g_game_state.player->get_position().x, top + height, 0.0f));
    }
    Entity* right_side = &g_game_state.platforms[2];
    if (g_game_state.player->check_collision(right_side))
    {
        success = true;
        g_game_state.player->set_acceleration(glm::vec3(0.0f));
        g_game_state.player->set_velocity(glm::vec3(0.0f));
        g_game_state.player->set_movement(glm::vec3(0.0f));
        close_time = SDL_GetTicks();
        float top = right_side->get_position().y + (right_side->get_scale().y / 2);
        float height = g_game_state.player->get_scale().y / 2;
        g_game_state.player->set_position(glm::vec3(g_game_state.player->get_position().x, top + height, 0.0f));
    }
    // Close game after 4 seconds
    if (failure && SDL_GetTicks() - close_time >= 4000)
    {
        SDL_Event quit_event;
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
    }
    if (success && SDL_GetTicks() - close_time >= 4000)
    {
        SDL_Event quit_event;
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
    }
}

void render()
{
    // ————— GENERAL ————— //
    glClear(GL_COLOR_BUFFER_BIT);

    // ————— PLAYER ————— //
    g_game_state.player->render(&g_shader_program);

    // ————— PLATFORM ————— //
    for (int i = 0; i < PLATFORM_COUNT; i++)
        g_game_state.platforms[i].render(&g_shader_program);

    // ————— SUCCESS AND FAILURE ————— //
    /*if (failure == true)
    {
        glBindTexture(GL_TEXTURE_2D, mission_failure);

        float vertices[] = {
            -2.0f, -0.5f, 1.0f, -0.5f, 1.0f, 0.5f,
            -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 0.5f
        };

        float tex_coords[] = {
            0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
        };

        glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(g_shader_program.get_position_attribute());

        glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
        glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(g_shader_program.get_position_attribute());
        glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    }*/
    if (failure && mission_failed)
    {
        mission_failed->render(&g_shader_program);
    }
    if (success && mission_succeeded)
    {
        mission_succeeded->render(&g_shader_program);
    }

    // ————— GENERAL ————— //
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete   g_game_state.player;
    delete[] g_game_state.platforms;
}


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
