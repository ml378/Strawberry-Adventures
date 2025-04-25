/**
* Author: Mahin Lalani
* Assignment: Strawberry Adventures
* Date due: 4/25/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Map.h"
#include <map>

Map::Map(int width, int height, unsigned int *level_data, GLuint texture_id, float tile_size, int tile_count_x, int tile_count_y) : 
m_width(width), m_height(height), m_level_data(level_data), m_texture_id(texture_id), m_tile_size(tile_size), m_tile_count_x(tile_count_x), m_tile_count_y(tile_count_y)
{ }

void Map::build()
{
    m_vertices.clear();
    m_texture_coordinates.clear();

    float map_width_world = m_width * m_tile_size;
    float map_height_world = m_height * m_tile_size;

    float x_origin = -map_width_world / 2.0f;
    float y_origin = map_height_world / 2.0f;

    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            int tile_id = m_level_data[y * m_width + x];
            if (m_uv_coords.find(tile_id) == m_uv_coords.end()) continue;

            TileUV uv = m_uv_coords[tile_id];
            
            float x_offset = x_origin + (x * m_tile_size);
            float y_offset = y_origin - (y * m_tile_size);

            m_vertices.insert(m_vertices.end(), {
                x_offset, y_offset,
                x_offset + m_tile_size, y_offset,
                x_offset + m_tile_size, y_offset - m_tile_size,
                x_offset, y_offset,
                x_offset + m_tile_size, y_offset - m_tile_size,
                x_offset, y_offset - m_tile_size
                });
            
            m_texture_coordinates.insert(m_texture_coordinates.end(), {
                uv.u, uv.v,
                uv.u + uv.w, uv.v,
                uv.u + uv.w, uv.v + uv.h,
                uv.u, uv.v,
                uv.u + uv.w, uv.v + uv.h,
                uv.u, uv.v + uv.h
                });
        }
    }
    
    // The bounds are dependent on the size of the tiles
    m_left_bound   = 0 - (m_tile_size / 2);
    m_right_bound  = (m_tile_size * m_width) - (m_tile_size / 2);
    m_top_bound    = 0 + (m_tile_size / 2);
    m_bottom_bound = -(m_tile_size * m_height) + (m_tile_size / 2);
}

void Map::render(ShaderProgram *program)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);
    
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, m_vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, m_texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    
    glDrawArrays(GL_TRIANGLES, 0, (int) m_vertices.size() / 2);
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Map::set_uv_coords(const std::map<int, TileUV> &uv_coords) {
    m_uv_coords = uv_coords;
}

int Map::get_current_tile(glm::vec3 position) const {
    if (position.x < m_left_bound || position.x > m_right_bound ||
        position.y > m_top_bound || position.y < m_bottom_bound) return -1;

    int tile_x = floor((position.x + (m_tile_size / 2)) / m_tile_size);
    int tile_y = -(ceil(position.y - (m_tile_size / 2))) / m_tile_size;

    if (tile_x < 0 || tile_x >= m_width || tile_y < 0 || tile_y >= m_height) return -1;

    return m_level_data[tile_y * m_width + tile_x];
}

void Map::update_tile(int x, int y, unsigned int new_tile) {
    m_level_data[y * m_width + x] = new_tile;
    build();
}
