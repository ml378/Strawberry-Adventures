/**
* Author: Mahin Lalani
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
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

    void initialise() override {
        font_texture_id = Utility::load_texture("assets/font1.png");
        m_game_state.next_scene_id = -1;
    }

    void update(float delta_time) override { }

    void render(ShaderProgram* program) override {
        Utility::draw_text(program, font_texture_id, "Press Enter to Start", 0.5f, -0.23f, glm::vec3(2.4f, -3.5f, 0.0f));
    }
};

Scene* create_menu_scene() {
    return new Menu();
}
