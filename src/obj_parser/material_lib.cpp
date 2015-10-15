#include <glm/glm.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "obj_parser/material_lib.hpp"
#include "extra/utility.hpp"

material_lib::material_lib(std::string&& dir)
    : dir_(dir)
{
}

auto
material_lib::get_materials() -> materials
{
    return materials_;
}

material_lib::~material_lib()
{
  // We can't delete these as they are used by our Object
  // for (auto& p: materials_)
  //   delete p.second;
}

void material_lib::update_material(material_ptr mtl, std::string& token)
{
    using namespace utility;

    if (!token.compare("map_Ka"))
        mtl->get_ambient_path() = std::string(dir_ + unix_file(iss_));
    else if (!token.compare("map_Kd"))
        mtl->get_diffuse_path() = std::string(dir_ + unix_file(iss_));
    else if (!token.compare("map_Ks"))
        mtl->get_specular_path() = std::string(dir_ + unix_file(iss_));
    else if (!token.compare("map_d"))
        mtl->get_dissolve_path() = std::string(dir_ + unix_file(iss_));
    else if (!token.compare("map_bump"))
        mtl->get_bump_path() = std::string(dir_ + unix_file(iss_));
    else if (!token.compare("map_d"))
        mtl->get_dissolve_path() = std::string(dir_ + unix_file(iss_));
    else if (!token.compare("bump"))
        mtl->get_bump_path() = std::string(dir_ + unix_file(iss_));
    else if (!token.compare("Ka"))
        mtl->get_ambient() = make_vec3(iss_, "ambient_mat");
    else if (!token.compare("Kd"))
        mtl->get_diffuse() = make_vec3(iss_, "diffuse_mat");
    else if (!token.compare("Ke"))
        mtl->get_emissive() = make_vec3(iss_, "emissive_mat");
    else if (!token.compare("Ks"))
        mtl->get_specular() = make_vec3(iss_, "specular_mat");
    else if (!token.compare("Ns"))
        mtl->get_shininess() = make_float(iss_, "shininess");
    else if (!token.compare("d"))
        mtl->get_dissolve() = make_float(iss_, "dissolve");
}

void
material_lib::add_material(material* mtl, std::string& material_name)
{
    // All objects are vnta type now we might need a way to check it
    //assert(mtl->get_bump_path().empty() && !mtl->get_dissolve_path().empty());
    mtl->set_render_type(render_type::bump_dissolve);
    materials_[material_name] = mtl;
}

void
material_lib::load_material_lib(std::istringstream& iss)
{
    std::string filename, token;
    iss >> filename;
    ifs_.open(dir_ + filename);
    if (!ifs_.good()) {
        std::cerr << "Unable loading material lib : " << dir_ + filename
                  << std::endl;
        return;
    }
    std::string buff;
    std::string material_name;
    std::getline(ifs_, buff);
    material_ptr mtl = nullptr;
    while (!ifs_.eof()) {
        iss_.str(buff);
        iss_ >> token;
        if (!token.compare("newmtl")) {
            if (mtl) {
                add_material(mtl, material_name);
            }
            mtl = new material();
            iss_ >> material_name;
            std::transform(material_name.begin(), material_name.end(),
                           material_name.begin(), ::tolower);
            materials_[material_name] = mtl;
        }
        else if (mtl)
            update_material(mtl, token);

        std::getline(ifs_, buff);
        token.clear();
        iss_.clear();
    }
    add_material(mtl, material_name);
    ifs_.close();
}

void
material_lib::dump()
{
    for (auto mat : materials_) {
        std::cout << mat.first << std::endl;
        material* ptr = mat.second;
        std::cout << "\tshininess"    << ptr->get_shininess() << std::endl;
        std::cout << "\tambient\t"; utility::print(ptr->get_ambient());
        std::cout << "\tdiffuse\t"; utility::print(ptr->get_diffuse());
        std::cout << "\tspecular\t"; utility::print(ptr->get_specular());
        std::cout << "\tambient_map\t"  << ptr->get_ambient_path() << std::endl;
        std::cout << "\tdiffuse_map\t"  << ptr->get_diffuse_path() << std::endl;
        std::cout << "\tspecular_map\t" << ptr->get_specular_path() << std::endl;
        std::cout << "\tbump_map\t"     << ptr->get_bump_path() << std::endl;
        std::cout << "\tbump\t"         << ptr->get_bump_path() << std::endl;
        std::cout << "\tmap_d\t"        << ptr->get_dissolve_path() << std::endl;
    }
}

material*
material_lib::get_material(std::string& material_name)
{
    auto it = materials_.find(material_name);
    if (it == materials_.end()) {
        it = materials_.find("");
        if (it == materials_.end())
            materials_[""] = new material;
        std::cerr << "Unable to find material name : " << material_name
                  << std::endl;
        // Default material when none is found
        return materials_[""];
    }
    return (*it).second;
}


boundary
get_boundary(material& mat)
{
  boundary res;
  using vertex_t = typename material::value_type;
  const std::vector<vertex_t> vertices = mat.get_vertices();
  if (vertices.empty())
    return res;
  assert(!vertices.empty() && "Vertices must be filled first");
  float min_x = vertices[0].v.x;
  float min_y = vertices[0].v.y;
  float max_x = vertices[0].v.x;
  float max_y = vertices[0].v.y;
  float max_z = vertices[0].v.z;

  for (auto& v: vertices)
    {
      min_x = std::min(min_x, v.v.x);
      max_x = std::max(max_x, v.v.x);

      min_y = std::min(min_y, v.v.y);
      max_y = std::max(max_y, v.v.y);

      max_z = std::max(max_z, v.v.z);
    }
  res.width = max_x - min_x;
  res.height = max_y - min_y;
  res.look_at = glm::vec3((max_x + min_x) / 2, (max_y + min_y) / 2, max_z);
  return res;
}

voxel
intersect_ray(material& mat, glm::vec3& eye_pos, glm::vec3& eye_dir)
{
  using vertex_t = typename material::value_type;
  const std::vector<vertex_t>& vertices = mat.get_vertices();
  const auto& indices = mat.get_indices();
  voxel res;
  for (unsigned i = 0; i < indices.size(); i += 3)
    {
      const glm::vec3& v0 = vertices[indices[i]].v;
      const glm::vec3& v1 = vertices[indices[i + 1]].v;
      const glm::vec3& v2 = vertices[indices[i + 2]].v;

      const glm::vec3& n0 = vertices[indices[i]].n;
      const glm::vec3& n1 = vertices[indices[i + 1]].n;
      const glm::vec3& n2 = vertices[indices[i + 2]].n;
      // compute normal of triangle to determine it's plane
      glm::vec3 u = v1 - v0;
      glm::vec3 v = v2 - v0;
      // Computing cross enables better results as using the average of 3
      // normals generates up ta 5% mistakes due to innacurate normal
      glm::vec3 n = glm::normalize(glm::cross(u, v));
      float dot_val = -glm::dot(n, glm::normalize(eye_dir));
      // Ignore when ray is parallel to triangle or wrong side
      if (fabs(dot_val) <= std::numeric_limits<float>::epsilon())
        continue;
      // Find intersection on plane
      float r = -glm::dot(n, eye_pos - v0) / glm::dot(n, eye_dir);
      if (r <= 0.0f)
        continue;

      // The intersecting point
      glm::vec3 p = eye_pos + r * (eye_dir);
      // A vector from two points on plane to compute s and t parameters based
      // on u and v coordinate system
      glm::vec3 w = p - v0;
      float div = powf(glm::dot(u, v), 2) - glm::dot(u, u) * glm::dot(v, v);
      float s = glm::dot(u, v) * glm::dot(w, v) - glm::dot(v, v) * glm::dot(w, u);
      s /= div;
      float t = glm::dot(u, v) * glm::dot(w, u) - glm::dot(u, u) * glm::dot(w, v);
      t /= div;
      // Check if point was in triangle
      if (t >= 0.0f && s >= 0.0f && t + s <= 1.0f)
        {
          glm::vec3 interpolated_normal = glm::normalize(s * n1 + t * n2 +
                                                         (1 - (t + s)) * n0);
          float new_dist = glm::distance(p, eye_pos);
          // Object has to be closer but not too close
          if (new_dist < res.dist && new_dist > 0.01f)
            {
              res.dist = new_dist;
              res.norm = interpolated_normal;
              res.pos = v0 + s * u + t * v;
              res.mat = &mat;
            }
        }
    }

  return res;
}
