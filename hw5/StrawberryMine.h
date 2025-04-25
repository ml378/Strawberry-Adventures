#include "Scene.h"

class StrawberryMine : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    bool m_level_complete = false;
    bool m_loss = false;

    // Mining variables
    const static int WIDTH = 8;
    const static int HEIGHT = 8;

    enum BlockState { COVERED, UNCOVERED, OBJECT };
    BlockState m_grid[HEIGHT][WIDTH];
    Uint32 m_timer;
    int m_milk_x;
    int m_milk_y;
    int m_pos_x;
    int m_pos_y;

    bool m_success;
    bool m_failure;

    // ————— DESTRUCTOR ————— //
    ~StrawberryMine();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    bool is_complete() const;
    bool is_loss() const;
    void click(float pos_x, float pos_y);
};