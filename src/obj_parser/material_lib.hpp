#ifndef MATERIAL_LIB_HPP
# define MATERIAL_LIB_HPP

# include <glm/glm.hpp>
# include <string>
# include <sstream>
# include <fstream>
# include <unordered_map>
# include <vector>
# include <memory>
# include <tuple>

# include "extra/utility.hpp"
# include "obj_parser/material.hpp"

class material_lib
{
public:
    using materials = std::unordered_map<std::string, material*>;
    using material_ptr = material*;
    material_lib(std::string&& dir);
    // The material library using filename in the string stream
    void load_material_lib(std::istringstream& iss);
    // Returns pointer to a material for a given name or nullptr when not found.
    material_ptr get_material(std::string& mat_name);
    materials get_materials();
    ~material_lib();
    void dump();
private:
    void update_material(material_ptr mtl, std::string& token);
    void add_material(material* mtl, std::string& material_name);
    materials materials_;
    std::istringstream iss_;
    std::ifstream ifs_;
    std::string dir_;
};

voxel intersect_ray(material& mat, glm::vec3& eye_pos, glm::vec3& eye_dir);
boundary get_boundary(material& mat);

#endif // MATERIAL_LIB_HPP
