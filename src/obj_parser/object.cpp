#include "obj_parser/object.hpp"

void
object::add_material(material* mat)
{
    materials_.push_back(mat);
}

const glm::mat4&
object::get_model_mat()
{
    return model_mat_;
}

void
object::set_model_mat(glm::mat4& model_mat)
{
    model_mat_ = model_mat;
}


auto
object::get_materials() -> materials_t&
{
    return materials_;
}

void
object::translate(const glm::vec3& dir)
{
    model_mat_ = glm::translate(model_mat_, dir);
}

void
object::rotate(float degrees, const glm::vec3& dir)
{
    model_mat_ = glm::rotate(model_mat_, glm::radians(degrees), dir);
}

void
object::scale(const glm::vec3& vec)
{
    model_mat_ = glm::scale(model_mat_, vec);
}

void
object::set_render_mode(render_type type)
{
    for (auto mat : materials_)
        mat->set_render_type(type);
}

float
object::intersect_ray(glm::vec3& L, glm::vec3& ray, glm::vec3& color)
{
  float min_dist = FLT_MAX;
  for (auto& mat : materials_)
    {
      glm::vec3 tmp_color;
      float dist = mat->intersect_ray(L, ray, tmp_color);
      if (dist > 0 && min_dist > dist)
        {
          min_dist = dist;
          color = tmp_color;
        }
    }
  return min_dist;
}

boundary
object::get_boundary()
{
  boundary res;
  for (auto& mat: materials_)
    {
      res.merge(mat->get_boundary());
    }
  return res;
}

object::~object()
{
  for (auto& mat: materials_)
    delete mat;
}