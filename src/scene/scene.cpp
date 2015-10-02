#include <vector>

#include "scene/scene.hpp"

scene::scene(boundary& bound,
             unsigned x_res, unsigned y_res)
    : camera_(bound.look_at),
      width_(bound.width),
      height_(bound.height),
      x_res_(x_res),
      y_res_(y_res)
{
}

void
scene::add_object(object* object)
{
    objects_.push_back(object);
}

void
scene::render()
{
  float w = width_;
  float h = height_;
  float x_res = static_cast<float>(x_res_);
  float y_res = static_cast<float>(y_res_);
  const glm::vec3& u = camera_.get_right();
  const glm::vec3& v = camera_.get_up();
  glm::vec3 eye = camera_.get_pos_vec();
  // look_at is the center of the image
  glm::vec3 look_at = camera_.get_look_at();
  // L is the bottom left of image
  glm::vec3 L = look_at - u * w / 2.0f - v * h / 2.0f;

  std::vector<glm::vec3> res;
  for (unsigned y = y_res_; y > 0; --y)
    {
      for (unsigned x = 0; x < x_res_; ++x)
        {
          // ray is the current pixel at (x, y) from bottom left
          glm::vec3 ray = L + u * static_cast<float>(x) * w / x_res +
                              v * static_cast<float>(y - 1) * h / y_res;
          float min_dist = FLT_MAX;
          glm::vec3 color(255, 255, 255);
          for (auto& o: objects_)
            {
              // dist == FLT_MAX when nothing was found
              float dist = o->intersect_ray(eye, ray);
              if (min_dist > dist)
                {
                  min_dist = dist;
                  color = glm::vec3(0, 0, 0);
                }
            }
          // Note image is pushed backwards
          res.push_back(color);
        }
    }
  dump_to_file(res);
}

void
scene::dump_to_file(std::vector<glm::vec3>& vect)
{
  std::ofstream ofs;
  ofs.open("out.ppm");
  ofs << "P3" << std::endl;
  ofs << x_res_ << ' ' << y_res_ << std::endl;
  ofs << 255 << std::endl;
  for (auto& vec: vect)
    ofs << vec.x << "\t" << vec.y << "\t" << vec.z << std::endl;
  ofs.close();
}
