#include <vector>
#include <tbb/parallel_for.h>

#include "scene/scene.hpp"

// Passing bound.width to camera is a good estimate to place camera a distance
// of bound.width to look_at point
scene::scene(boundary& bound,
             unsigned x_res, unsigned y_res)
    : camera_(bound.look_at, bound.width),
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
scene::operator() (const tbb::blocked_range<unsigned>& r) const
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

  for (unsigned y = r.begin(); y != r.end(); ++y)
    {
      for (unsigned x = 0; x < x_res_; ++x)
        {
          // ray is the current pixel at (x, y) from bottom left
          glm::vec3 ray = L + u * static_cast<float>(x + 0.5f) * w / x_res +
                              v * static_cast<float>(0.5f + y_res_ - y - 1) * h / y_res;
          float min_dist = FLT_MAX;
          glm::vec3 color(0, 0, 0);
          for (auto& o: objects_)
            {
              // dist == FLT_MAX when nothing was found
              glm::vec3 tmp_color;
              float dist = o->intersect_ray(eye, ray, tmp_color);
              if (min_dist > dist)
                {
                  min_dist = dist;
                  color = tmp_color;
                }
            }
          // Note image is pushed backwards
          glm::vec3* res = const_cast<glm::vec3*>(res_.data());
          res[x + static_cast<unsigned>(y) * x_res_] = color;
        }
    }
}

void
scene::render()
{
  res_.resize(y_res_ * x_res_);
  std::cout << "Raytracing image\n";
  tbb::parallel_for(tbb::blocked_range<unsigned>(0, y_res_, 1), std::ref(*this));
  std::cout << "Writting image to out.ppm\n";
  dump_to_file();
}

void
scene::dump_to_file() const
{
  std::ofstream ofs;
  ofs.open("out.ppm");
  ofs << "P3" << std::endl;
  ofs << x_res_ << ' ' << y_res_ << std::endl;
  ofs << 255 << std::endl;
  for (auto& vec: res_)
    ofs << roundf(vec.x) << "\t" << roundf(vec.y) << "\t" << roundf(vec.z)
        << std::endl;
  ofs.close();
}
