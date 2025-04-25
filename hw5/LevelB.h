#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 1;
    bool m_level_complete = false;
    bool m_loss = false;
    float m_lose_timer = 0.0f;

    // ————— DESTRUCTOR ————— //
    ~LevelB();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    bool is_complete() const;
};
