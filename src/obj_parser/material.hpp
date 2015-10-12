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

class resource_manager;

class material
{
public:
    using container_vnt = std::vector<utility::vertex_vnt>;
    using container_vnta = std::vector<utility::vertex_vnta>;
    using container_vn = std::vector<utility::vertex_vn>;
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
    std::string& get_ambient_path();
    std::string& get_diffuse_path();
    std::string& get_specular_path();
    std::string& get_dissolve_path();
    std::string& get_bump_path();
    render_type get_render_type();
    void set_render_type(render_type type);
    vertices_idx& get_indices();
    index_map& get_idx_lut();

    virtual boundary get_boundary();
    virtual voxel intersect_ray(glm::vec3& L, glm::vec3& ray);
    virtual ~material() = default;
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

    // Associative map_ of all indices of object to check.  If index already
    // exists it's id can be return, otherwise a new one is created
    index_map idx_lut;

    // By defalut a material has no textures and only basic material color
    render_type render_type_ = render_type::color;
protected:
    // Associated indices to material, each pack of 3 indices represents a
    // triangle and the data is found in the vertex buffer whichi usually is on
    // the GPU in OpenGL
    vertices_idx indices;

public:
    // Hack to know when we use material_v
    // want_normal = true => normals will be computed from vertices
    bool want_normal = true;
};

class material_vnt : public material
{
public:
    material_vnt(material& parent)
        : material(parent)
    {}
    using container_vnt = std::vector<utility::vertex_vnt>;
    using value_type = utility::vertex_vnt;
    container_vnt& get_vertices();
    static const bool has_normal = 1;
    static const bool has_texture = 1;
    static const bool has_adjacent = 0;

    virtual voxel intersect_ray(glm::vec3& L, glm::vec3& ray) override;
    virtual boundary get_boundary() override;
    virtual ~material_vnt() = default;
private:
    container_vnt vertices_vnt;
};

class material_v : public material
{
public:
    // want_normal = true => normals will be computed from vertices
    material_v()
    { want_normal = true; }
    material_v(material_v&)
    { want_normal = true; }
    material_v(material& parent)
        : material(parent)
    { want_normal = true; }
    using container_v = std::vector<utility::vertex_v>;
    using value_type = utility::vertex_v;
    container_v& get_vertices();
    static const bool has_normal = 0;
    static const bool has_texture = 0;
    static const bool has_adjacent = 0;

    virtual boundary get_boundary() override;
    virtual voxel intersect_ray(glm::vec3& L, glm::vec3& ray) override;
    virtual ~material_v() = default;
private:
    container_v vertices_v;
};

class material_vn : public material
{
public:
    material_vn() = default;
    material_vn(material& parent)
        : material(parent)
    {}
    using container_vn = std::vector<utility::vertex_vn>;
    using value_type = utility::vertex_vn;
    container_vn& get_vertices();
    static const bool has_normal = 1;
    static const bool has_texture = 0;
    static const bool has_adjacent = 0;

    virtual boundary get_boundary() override;
    virtual voxel intersect_ray(glm::vec3& L, glm::vec3& ray) override;
    virtual ~material_vn() = default;
private:
    container_vn vertices_vn;
};

class material_vnta : public material
{
public:
    material_vnta(material& parent)
        : material(parent)
    {}
    using container_vnta = std::vector<utility::vertex_vnta>;
    using value_type = utility::vertex_vnta;
    material_vnta() = default;
    container_vnta& get_vertices();
    static const bool has_normal = 1;
    static const bool has_texture = 1;
    static const bool has_adjacent = 1;

    virtual boundary get_boundary() override;
    virtual voxel intersect_ray(glm::vec3& L, glm::vec3& ray) override;
    virtual ~material_vnta() = default;
private:
    container_vnta vertices_vnta;
};

#include "obj_parser/material_lib.hxx"

#endif // MATERIAL_HPP
