#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 5;
    bool m_level_complete = false;
    bool m_loss = false;
    bool m_life_lost = false;
    float m_life_loss_timer = 0.0f;

    // ————— DESTRUCTOR ————— //
    ~LevelB();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    bool is_complete() const;
    bool is_loss() const;
};
