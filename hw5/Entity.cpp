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
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

static int walk_left[8] = { 8, 9, 10, 11, 12, 13, 14, 15 };
static int walk_right[8] = { 16, 17, 18, 19, 20, 21, 22, 23 };
static int idle_left[2] = { 0, 1 };
static int idle_right[2] = { 2, 3 };

void Entity::ai_activate(Entity* player, float delta_time)
{
    switch (m_ai_type)
    {
    case WALKER:
        ai_walker(delta_time);
        break;
    case WALKER_BOSS:
        ai_walker_boss(player, delta_time);
        break;
    case GUARD:
        ai_guard(player);
        break;
    case FLYER:
        ai_flyer(player, 0.016f);
        break;
    case PROJECTILE:
        ai_projectile(delta_time);
        break;
    default:
        break;
    }
}

void Entity::ai_walk()
{
    m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
}
void Entity::ai_walker(float delta_time)
{
    m_walk_timer += delta_time;
    m_w_direction_timer += delta_time;

    if (m_w_direction_timer >= m_w_direction_interval)
    {
        m_w_direction *= -1;
        m_w_direction_timer = 0.0f;
    }

    m_movement.x = m_w_direction;
}

void Entity::ai_walker_boss(Entity* player, float delta_time) {
    switch (m_ai_state) {
    case IDLE:
        if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = WALKING;
        break;

    case WALKING:
        m_walk_timer += delta_time;
        m_w_direction_timer += delta_time;

        if (m_w_direction_timer >= m_w_direction_interval)
        {
            m_w_direction *= -1;
            m_w_direction_timer = 0.0f;
        }

        m_movement.x = m_w_direction;
        break;

    default:
        break;
    }
}
void Entity::ai_guard(Entity* player)
{
    switch (m_ai_state) {
    case IDLE:
        if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = WALKING;
        break;

    case WALKING:
        glm::vec3 direction = player->get_position() - m_position;
\
        if (glm::length(direction) > 0.0f) {
            direction = glm::normalize(direction);
        }

        m_movement = glm::vec3(direction.x, direction.y, 0.0f);
        break;

    case ATTACKING:
        break;

    default:
        break;
    }
}
void Entity::ai_flyer(Entity* player, float delta_time)
{
    float frequency = 1.5f;
    float radius = 1.0f;
    switch (m_ai_state) {
    case IDLE:
        if (glm::distance(m_position, player->get_position()) < 4.0f) m_ai_state = WALKING;
        break;

    case WALKING:
        m_fly_timer += delta_time;
        m_movement.x = cos(m_fly_timer * frequency) * radius;
        m_movement.y = sin(m_fly_timer * frequency) * radius;

    default:
        break;
    }
}
void Entity::ai_projectile(float delta_time)
{
    m_motion_timer += delta_time;
    m_movement.x = 2.0f;
    if (m_motion_timer >= m_motion_interval)
    {
        set_position(m_start_pos);
        m_motion_timer = 0.0f;
    }
}
// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

// Parameterized constructor
Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[4][4], float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    set_walking(walking);
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState) : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

Entity::~Entity() {}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                m_position.y -= y_overlap;
                m_velocity.y = 0;

                // Collision!
                m_collided_top = true;
            }
            else if (m_velocity.y < 0)
            {
                m_position.y += y_overlap;
                m_velocity.y = 0;

                // Collision!
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x -= x_overlap;
                m_velocity.x = 0;

                // Collision!
                m_collided_right = true;

            }
            else if (m_velocity.x < 0)
            {
                m_position.x += x_overlap;
                m_velocity.x = 0;

                // Collision!
                m_collided_left = true;
            }
        }
    }
}

void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map, Map* grass)
{
    if (glm::length(m_movement) > 0.1f && m_entity_type == PLAYER) {
        if (fabs(m_movement.x) > fabs(m_movement.y)) {
            if (m_movement.x > 0) {
                m_animation_indices = walk_right;
                m_animation_frames = 8;
            }
            else {
                m_animation_indices = walk_left;
                m_animation_frames = 8;
            }
        }
        else {
            if (m_movement.y > 0) {
                m_animation_indices = walk_right;
                m_animation_frames = 8;
            }
            else {
                m_animation_indices = walk_left;
                m_animation_frames = 8;
            }
        }
        m_animation_time += delta_time;
        if (m_animation_time >= 0.15f) {
            m_animation_time = 0.0f;
            m_animation_index++;
            if (m_animation_index >= m_animation_frames) m_animation_index = 0;
        }
    }
    else {
        m_animation_index = 0;
    }

    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_entity_type == ENEMY) ai_activate(player, delta_time);

    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;

    m_velocity.y = m_movement.y * m_speed;
    m_position.y += m_velocity.y * delta_time;

    check_collision_y(collidable_entities, collidable_entity_count);

    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);

    int tile = grass->get_current_tile(glm::vec3(m_position.x, m_position.y - (m_height / 2) - 0.01f, m_position.z));

    if (tile == 20) {
        // mining
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}


void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        if (m_costume_id != 0 && m_entity_type == PLAYER)
        {
            draw_sprite_from_texture_atlas(program, m_costume_id, m_animation_indices[m_animation_index]);
        }
        return;
    }
    else if (m_animation_cols > 0 && m_animation_rows > 0)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_index);
        if (m_costume_id != 0 && m_entity_type == PLAYER)
        {
            draw_sprite_from_texture_atlas(program, m_costume_id, m_animation_index);
        }
    }
    else {
        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);

        if (m_costume_id != 0 && m_entity_type == PLAYER)
        {
            glBindTexture(GL_TEXTURE_2D, m_costume_id);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
}
