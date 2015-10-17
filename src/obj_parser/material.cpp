#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

#include "obj_parser/material.hpp"
#include "extra/utility.hpp"

auto material::get_vertices() -> container_vnta&
{
    return vertices_vnta;
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

float& material::get_Ni()
{
    return Ni;
}

unsigned int& material::get_illum()
{
    return illum;
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
material::intersect_ray(kdtree* tree, glm::vec3& pos, glm::vec3& dir,
                        glm::vec3& inv_dir)
{
  if (!tree)
    return voxel();

  if (tree->box_.intersect(pos, inv_dir))
    {
      if (tree->right_ == tree->left_)
        {
          // TODO at the moment we only have bounding boxes
          kdtree_leaf* leaf = static_cast<kdtree_leaf*>(tree);
          return ::intersect_ray(*this, pos, dir, leaf->indices_);
        }
      else
        {
          voxel v0 = intersect_ray(tree->left_, pos, dir, inv_dir);
          voxel v1 = intersect_ray(tree->right_, pos, dir, inv_dir);
          return v0.dist < v1.dist ? v0 : v1;
        }
    }
  else
    return voxel();
}

voxel
material::intersect_ray(glm::vec3& pos, glm::vec3& dir)
{
  glm::vec3 inv_dir;
  inv_dir.x = fabsf(dir.x) < 0.001f ? FLT_MAX : 1.f / dir.x;
  inv_dir.y = fabsf(dir.y) < 0.001f ? FLT_MAX : 1.f / dir.y;
  inv_dir.z = fabsf(dir.z) < 0.001f ? FLT_MAX : 1.f / dir.z;

  return intersect_ray(tree_, pos, dir, inv_dir);
}

boundary
material::get_boundary()
{
  return ::get_boundary(*this);
}

void material::compute_kdtree()
{
  assert(!tree_ && "only call this function once");
  tree_ = create_kdtree(indices, get_vertices());
}
