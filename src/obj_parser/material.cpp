#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

#include "obj_parser/material.hpp"
#include "extra/utility.hpp"

auto material_vnta::get_vertices() -> container_vnta&
{
    return vertices_vnta;
}
auto material_vnt::get_vertices() -> container_vnt&
{
    return vertices_vnt;
}
auto material_vn::get_vertices() -> container_vn&
{
    return vertices_vn;
}
auto material_v::get_vertices() -> container_v&
{
    return vertices_v;
}

////////////////////////////////////
// MATERIAL BASE CLASS DEFINITION //
////////////////////////////////////

material::material(render_type type)
    : render_type_(type)
{
}

glm::vec3&
material::get_ambient()
{
    return ambient_;
}

glm::vec3&
material::get_specular()
{
    return specular_;
}

glm::vec3&
material::get_emissive()
{
    return emissive_;
}


glm::vec3&
material::get_diffuse()
{
    return diffuse_;
}

float&
material::get_shininess()
{
    return shininess;
}

std::string& material::get_ambient_path()
{
    return ambient_path_;
}

std::string& material::get_diffuse_path()
{
    return diffuse_path_;
}

std::string& material::get_specular_path()
{
    return specular_path_;
}

std::string& material::get_dissolve_path()
{
    return dissolve_path_;
}

std::string& material::get_bump_path()
{
    return bump_path_;
}

float& material::get_dissolve()
{
    return dissolve;
}

auto material::get_idx_lut() -> index_map&
{
    return idx_lut;
}

auto material::get_indices() -> vertices_idx&
{
    return indices;
}

render_type
material::get_render_type()
{
    return render_type_;
}

void
material::set_render_type(render_type type)
{
    render_type_ = type;
}

voxel
material::intersect_ray(glm::vec3& L, glm::vec3& ray)
{
  (void) ray;
  (void) L;
  // Ideally this class should be virtual void, however obj_loading uses a hack
  // that needs to first instanciate a metrial class, as at the time of
  // creation, obj_loader does not know what type of material to create.
  std::cerr << "Material : material::intersect_ray should never be called as"
               " this class should never be instanciated!!\n";
  exit(1);
}

boundary
material::get_boundary()
{
  // Ideally this class should be virtual void, however obj_loading uses a hack
  // that needs to first instanciate a metrial class, as at the time of
  // creation, obj_loader does not know what type of material to create.
  std::cerr << "Material : material::intersect_ray should never be called as"
               " this class should never be instanciated!!\n";
  exit(1);
}

voxel
material_v::intersect_ray(glm::vec3& L, glm::vec3& look_at)
{
  return ::intersect_ray(*this, L, look_at);
}

voxel
material_vn::intersect_ray(glm::vec3& L, glm::vec3& look_at)
{
  return ::intersect_ray(*this, L, look_at);
}

voxel
material_vnt::intersect_ray(glm::vec3& L, glm::vec3& look_at)
{
  return ::intersect_ray(*this, L, look_at);
}

voxel
material_vnta::intersect_ray(glm::vec3& L, glm::vec3& look_at)
{
  return ::intersect_ray(*this, L, look_at);
}

boundary
material_v::get_boundary()
{
  return ::get_boundary(*this);
}

boundary
material_vn::get_boundary()
{
  return ::get_boundary(*this);
}

boundary
material_vnt::get_boundary()
{
  return ::get_boundary(*this);
}

boundary
material_vnta::get_boundary()
{
  return ::get_boundary(*this);
}
