/**
* Author: Mahin Lalani
* Assignment: Simple 2D Scene
* Date due: 2025-02-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
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

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH = 640 * 2,
WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED = 0.9765625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1,
LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

constexpr char CAT_SPRITE_FILEPATH[] = "cat.png",
WHITECAT_SPRITE_FILEPATH[] = "whitecat.png";

constexpr glm::vec3 INIT_SCALE = glm::vec3(1.67f, 1.99f, 0.0f),
INIT_POS_CAT = glm::vec3(2.0f, 0.0f, 0.0f),
INIT_POS_WHITECAT = glm::vec3(-2.0f, 0.0f, 0.0f);

constexpr float ROT_INCREMENT = 1.0f;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
g_cat_matrix,
g_whitecat_matrix,
g_projection_matrix;

float g_previous_ticks = 0.0f;

glm::vec3 g_rotation_cat = glm::vec3(0.0f, 0.0f, 0.0f),
g_rotation_whitecat = glm::vec3(0.0f, 0.0f, 0.0f);

GLuint g_cat_texture_id,
g_whitecat_texture_id;


GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Hello, Cats!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        SDL_Quit();
        exit(1);
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_cat_matrix = glm::mat4(1.0f);
    g_whitecat_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_cat_matrix = glm::translate(g_cat_matrix, INIT_POS_CAT);
    g_whitecat_matrix = glm::translate(g_whitecat_matrix, INIT_POS_WHITECAT);
    g_cat_matrix = glm::scale(g_cat_matrix, INIT_SCALE);
    g_whitecat_matrix = glm::scale(g_whitecat_matrix, INIT_SCALE);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    g_cat_texture_id = load_texture(CAT_SPRITE_FILEPATH);
    g_whitecat_texture_id = load_texture(WHITECAT_SPRITE_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}


void update()
{
    /* Delta Time Calculations */
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    /* Rotation Configurations */
    float rotation_speed = 90.0f;
    g_rotation_cat.y += glm::radians(rotation_speed) * delta_time;
    g_rotation_whitecat.y += glm::radians(rotation_speed) * delta_time;

    /* Circular Motion */
    float orbit_radius = 2.5f;  
    float orbit_speed = 0.75f; 
    float angle = ticks * orbit_speed;
    float cat_x = orbit_radius * cos(angle);
    float cat_y = orbit_radius * sin(angle);
    float whitecat_x = orbit_radius * cos(angle + glm::pi<float>());
    float whitecat_y = orbit_radius * sin(angle + glm::pi<float>());

    /* Model matrix reset */
    g_cat_matrix = glm::mat4(1.0f);
    g_whitecat_matrix = glm::mat4(1.0f);

    /* Translation */
    g_cat_matrix = glm::translate(g_cat_matrix, glm::vec3(cat_x, cat_y, 0.0f));
    g_whitecat_matrix = glm::translate(g_whitecat_matrix, glm::vec3(whitecat_x, whitecat_y, 0.0f));

    /* Apply Rotation */
    g_cat_matrix = glm::rotate(g_cat_matrix, g_rotation_cat.y, glm::vec3(0.0f, 1.0f, 0.0f));
    g_whitecat_matrix = glm::rotate(g_whitecat_matrix, g_rotation_whitecat.y, glm::vec3(0.0f, 1.0f, 0.0f));

    /* Scaling */
    g_cat_matrix = glm::scale(g_cat_matrix, INIT_SCALE);
    g_whitecat_matrix = glm::scale(g_whitecat_matrix, INIT_SCALE);
}


void draw_object(glm::mat4& object_g_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
        0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
        false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    draw_object(g_cat_matrix, g_cat_texture_id);
    draw_object(g_whitecat_matrix, g_whitecat_texture_id);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


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
