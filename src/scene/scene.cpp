#include <vector>
#include <tbb/parallel_for.h>

#include "scene/scene.hpp"

std::atomic<unsigned short> scene::counter(0);

// Passing bound.width to camera is a good estimate to place camera a distance
// of bound.width to look_at point
scene::scene(boundary& bound,
             unsigned int x_res, unsigned int y_res)
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
scene::operator() (const tbb::blocked_range<unsigned int>& r) const
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

  glm::vec3* res = const_cast<glm::vec3*>(res_.data());
  for (unsigned int y = r.begin(); y != r.end(); ++y)
    {
      for (unsigned int x = 0; x < x_res_; ++x)
        {
          // eye_look is the current pixel at (x, y) from bottom left
          glm::vec3 eye_look = L + u * static_cast<float>(x + 0.5f) * w / x_res +
                              v * static_cast<float>(0.5f + y_res_ - y - 1) *
                                  h / y_res;

          unsigned int idx = x + static_cast<unsigned int>(y) * x_res_;
          auto dir = glm::normalize(eye_look - eye);
          res[idx] = sample_pixel(eye, dir, 4);
        }
    }

  scene::counter.fetch_add(static_cast<unsigned short>(r.end() - r.begin()));
  short n0 = static_cast<short>(100.f * (scene::counter - 1) / y_res_);
  short n1 = static_cast<short>(100.f * (scene::counter) / y_res_);
  if (n0 != n1)
    (std::cout << n1 << ' ').flush();
}

// dir points towards objects
glm::vec3
scene::sample_pixel(glm::vec3& pos, glm::vec3& dir, unsigned int depth) const
{
  glm::vec3 color(0.f, 0.f, 0.f);
  voxel v = intersect_ray(pos, dir);
  // If nothing was intersected return black
  if (!v.mat)
    return (color);

  // reflective vector wrt dir
  glm::vec3 reflect = glm::normalize(dir - 2.f * glm::dot(v.norm, dir) * v.norm);
  // Compute reflected color
  if (depth)
    {
      // Offset pos slightly to avoid numerical errors otherwise we might
      // intersect with ourself
      auto p = v.pos + 0.01f * reflect;
      color += v.mat->get_specular() * sample_pixel(p, reflect, depth - 1);
    }
  for (const auto& l: dir_lights_)
    {
      color += v.mat->get_ambient() * l->get_ambient();
      float dot_diffuse = glm::dot(v.norm, l->get_position());
      if (dot_diffuse > 0.0f)
        {
          float dot_reflect = std::max(0.f, glm::dot(reflect,
                                                      l->get_position()));
          color += dot_diffuse * v.mat->get_diffuse() * l->get_diffuse();
          color += powf(dot_reflect, v.mat->get_shininess()) *
                   l->get_specular() * v.mat->get_specular();
        }
    }
  for (const auto& l: pos_lights_)
    {
      // Cheat since inderect lighting is not handled
      color += v.mat->get_ambient() * l->get_ambient();

      glm::vec3 light_dir = glm::normalize(l->get_position() - v.pos);

      auto p = l->get_position(); auto d = -light_dir;
      voxel c = intersect_ray(p, d);
      if (c.dist + 0.01f < glm::distance(l->get_position(), v.pos))
        continue;

      float dot_diffuse = glm::dot(v.norm, light_dir);
      if (dot_diffuse > 0.0f)
        {
          float dot_reflect = std::max(0.f, glm::dot(reflect, light_dir));
          color += dot_diffuse * v.mat->get_diffuse() * l->get_diffuse();
          color += powf(dot_reflect, v.mat->get_shininess()) *
                   l->get_specular() * v.mat->get_specular();
        }
    }
  color.x = std::min(1.f, color.x);
  color.y = std::min(1.f, color.y);
  color.z = std::min(1.f, color.z);
  // TODO Do other lights;

  return (color);
}

voxel
scene::intersect_ray(glm::vec3& pos, glm::vec3& dir) const
{
  voxel res;
  for (auto& o: objects_)
    {
      for (const auto& mat : o->get_materials())
        {
          voxel other = mat->intersect_ray(pos, dir);
          if (other.dist < res.dist)
            res = other;
        }
    }
  return res;
}

void
scene::render()
{
  scene::counter = 0;
  res_.resize(y_res_ * x_res_);
  std::cout << "Raytracing image\n";
  tbb::parallel_for(tbb::blocked_range<unsigned int>(0, y_res_, 1), std::ref(*this));
  std::cout << "\nWritting image to out.ppm\n";
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
  for (auto vec: res_)
    {
      vec *= 255.f;
      ofs << roundf(vec.x) << "\t" << roundf(vec.y) << "\t" << roundf(vec.z)
      << std::endl;
    }
  ofs.close();
}
