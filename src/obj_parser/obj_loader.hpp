#ifndef OBJ_LOADER_HPP
# define OBJ_LOADER_HPP

# include <vector>
# include <iostream>
# include <memory>
# include <sstream>
# include <fstream>
# include <unordered_set>

# include "obj_parser/object.hpp"
# include "obj_parser/material.hpp"
# include "extra/utility.hpp"

// A structure used for storing the indices of each vertex in the face
// declaration of a polygon.
struct s_vertex_idx
{
    // Vertex index
    size_t v;
    // Texture index
    size_t t;
    // Normal index
    size_t n;
};

class obj_loader
{
public:
    using index_map = std::unordered_map<std::tuple<size_t, size_t, size_t>,
                                         unsigned, hash_ptr>;
    using container_vnta = std::vector<utility::vertex_vnta>;
    using container_vnt = std::vector<utility::vertex_vnt>;
    using container_vn = std::vector<utility::vertex_vn>;
    using vertices_idx = std::vector<unsigned>;
    using materials = std::vector<material*>;
    using container3 = std::vector<glm::vec3>;
    using container2 = std::vector<glm::vec2>;
    using container_idx = std::vector<s_vertex_idx>;
    // Reads an Obj files and stores vertices, normals, and texture coords.
    // A simple call to glDrawArarys will render the object.
    object* load_obj(std::string& file);
    obj_loader(bool draw_line = true)
        : draw_lines_(draw_line)
    { }

    // Print out vector in the mesh format
    // Mesh will only contain triangles
    void print_results();

    // Dump out the contents of each vector
    void print_triangles(container3& vertices, container2& text_coord,
                         container3& normals);
private:
    void get_vertex(std::string& str, s_vertex_idx &v_idx);
    void add_indices();
    template <typename material>
    void index_object(material& mat);
    void compute_flat_shading(unsigned i, glm::vec3& cross);
    void compute_smooth_shading(std::vector<float>& normals_count,
                                size_t idx1, size_t idx2, size_t idx3,
                                glm::vec3& cross,
                                unsigned i);
    void compute_tangents(material_vnta& mat);
    void compute_normals(char flat_shading);
    void set_material_indices(material* mat);

    container3 vertices_;
    container3 normals_;
    container2 text_coords_;
    container_idx indices_;
    std::istringstream iss_;
    std::ifstream ifs_;
    bool draw_lines_;
};

#endif // OBJ_LOADER_HPP
