#include "Scene.h"

class LevelC : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 8;
    bool m_loss = false;
    bool m_win = false;
    bool m_life_lost = false;
    float m_life_loss_timer = 0.0f;
    
    // ————— DESTRUCTOR ————— //
    ~LevelC();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    bool is_loss() const;
    bool is_won() const;
};
