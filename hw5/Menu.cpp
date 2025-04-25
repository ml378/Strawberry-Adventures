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

class Menu : public Scene {
public:
    GLuint font_texture_id;
    GLuint background;


    void initialise() override {
        font_texture_id = Utility::load_texture("assets/font1.png");
        m_game_state.next_scene_id = -1;
        background = Utility::load_texture("assets/background.jpg");
    }

    void update(float delta_time) override { }

    void render(ShaderProgram* program) override {
        glm::mat4 model_matrix = glm::mat4(1.0f);
        glm::mat4 view_matrix = glm::mat4(1.0f);
        program->set_model_matrix(model_matrix);
        program->set_view_matrix(view_matrix);
        Utility::render_background(program, background);
        Utility::draw_text(program, font_texture_id, "Strawberry Adventures", 0.75f, -0.18f, glm::vec3(-5.75f, 2.7f, 0.0f));
        Utility::draw_text(program, font_texture_id, "Press Enter to Start", 0.75f, -0.18f, glm::vec3(-5.4f, -1.3f, 0.0f));
    }
};

Scene* create_menu_scene() {
    return new Menu();
}