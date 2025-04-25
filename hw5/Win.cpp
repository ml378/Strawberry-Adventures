/**
* Author: Mahin Lalani
* Assignment: Strawberry Adventures
* Date due: 4/25/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Scene.h"
#include "Utility.h"

class Win : public Scene {
public:
    GLuint font_texture_id;
    GLuint background;
    Uint32 win_time = 0;


    void initialise() override {
        font_texture_id = Utility::load_texture("assets/font1.png");
        m_game_state.next_scene_id = -1;
        background = Utility::load_texture("assets/background.jpg");
        win_time = SDL_GetTicks();
    }

    void update(float delta_time) override {
        if (SDL_GetTicks() - win_time >= 4000) {
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
        }
    }

    void render(ShaderProgram* program) override {
        glm::mat4 model_matrix = glm::mat4(1.0f);
        glm::mat4 view_matrix = glm::mat4(1.0f);
        program->set_model_matrix(model_matrix);
        program->set_view_matrix(view_matrix);
        Utility::render_background(program, background);
        Utility::draw_text(program, font_texture_id, "You Win!", 0.75f, -0.18f, glm::vec3(-1.85f, 0.85f, 0.0f));
    }
};

Scene* create_win_scene() {
    return new Win();
}