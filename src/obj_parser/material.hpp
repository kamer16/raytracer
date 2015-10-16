#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include <glm/glm.hpp>
# include <string>
# include <sstream>
# include <fstream>
# include <unordered_map>
# include <vector>
# include <tuple>

# include "extra/utility.hpp"
# include "extra/kdtree.hpp"

class resource_manager;

class material
{
public:
    using value_type = utility::vertex_vnta;
    using container_vnta = std::vector<utility::vertex_vnta>;
    using index_map = std::unordered_map<std::tuple<size_t, size_t, size_t>,
          unsigned, hash_ptr>;
    using vertices_idx = std::vector<unsigned>;
    material(render_type type);
    material()
    { }
    material(material& other) = default;
    glm::vec3& get_ambient();
    glm::vec3& get_specular();
    glm::vec3& get_diffuse();
    glm::vec3& get_emissive();
    void set_ambient(glm::vec3&& ambient) { ambient_ = ambient; }
    void set_diffuse(glm::vec3&& diffuse) { diffuse_ = diffuse; }
    void set_specular(glm::vec3&& specular) { specular_ = specular; }
    void set_emissive(glm::vec3&& emissive) { emissive_ = emissive; }
    void set_ambient(glm::vec3& ambient) { ambient_ = ambient; }
    void set_diffuse(glm::vec3& diffuse) { diffuse_ = diffuse; }
    void set_specular(glm::vec3& specular) { specular_ = specular; }
    void set_emissive(glm::vec3& emissive) { emissive_ = emissive; }
    float& get_shininess();
    float& get_dissolve();
    float& get_Ni();
    unsigned int& get_illum();
    std::string& get_ambient_path();
    std::string& get_diffuse_path();
    std::string& get_specular_path();
    std::string& get_dissolve_path();
    std::string& get_bump_path();
    render_type get_render_type();
    void set_render_type(render_type type);
    vertices_idx& get_indices();
    void compute_kdtree();
    index_map& get_idx_lut();

    boundary get_boundary();
    voxel intersect_ray(glm::vec3& L, glm::vec3& ray);
    container_vnta& get_vertices();
    ~material() = default;
private:
    // TODO anmbient_ is a cheat and only needed when inderected lighting is not
    // computed, otherwise object only has diffuse color and shininess factor
    // and specular_ factor.
    // Ns
    float shininess = 0.0f;
    // Ka
    glm::vec3 ambient_;
    // Kd
    glm::vec3 diffuse_;
    // Ks
    glm::vec3 specular_;
    // Ke
    glm::vec3 emissive_ = glm::vec3(0.f, 0.f, 0.f);
    // ambient texture map ==> map_Ka
    std::string ambient_path_;
    // diffuse texture map ==> map_Kd
    std::string diffuse_path_;
    // specular texture map ==> map_Ks
    std::string specular_path_;
    // dissolve texture map ==> map_d
    std::string dissolve_path_;
    // Bump texture map ==> map_bump
    std::string bump_path_;
    float dissolve = 1.0f;
    // Thre illumination models handles 2 is diffuse+spec, 5 only reflective, 7
    // transparent with reflectionc and refraction
    unsigned illum = 2;
    // Refractive index, used when passing through transparent object
    // By debault we use a value of 1 == Ni of air.
    float Ni = 1;

    // Associative map_ of all indices of object to check.  If index already
    // exists it's id can be return, otherwise a new one is created
    index_map idx_lut;

    // By defalut a material has no textures and only basic material color
    render_type render_type_ = render_type::color;

    // Binary tree used to cut object in small cubes to avoid looking for all
    // triangles
    kdtree* tree_;
protected:
    // Associated indices to material, each pack of 3 indices represents a
    // triangle and the data is found in the vertex buffer whichi usually is on
    // the GPU in OpenGL
    vertices_idx indices;
    container_vnta vertices_vnta;

};

voxel intersect_ray(material& mat, glm::vec3& eye_pos, glm::vec3& eye_dir,
                    std::vector<unsigned int>& indices);
boundary get_boundary(material& mat);

#endif // MATERIAL_HPP
