#include <vector>
#include <cmath>
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
  unsigned short nb[3] = { 0, 0, counter };
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
          res[idx] = glm::vec3(0.f, 0.f, 0.f);
          int nb_samples = 32;
          if (basic_ray_tracing)
            nb_samples = 1;
          float div = static_cast<float>(nb_samples);
          unsigned depth = 5;
          for (int i = 0; i < nb_samples; ++i)
            res[idx] = res[idx] + sample_pixel(eye, dir, depth, nb) / div;

          res[idx].x = std::min(1.f, res[idx].x);
          res[idx].y = std::min(1.f, res[idx].y);
          res[idx].z = std::min(1.f, res[idx].z);

        }
    }

  auto c = scene::counter.fetch_add(static_cast<unsigned short>(r.end() - r.begin()));
  short n0 = static_cast<short>(100.f * (c - 1) / y_res_);
  short n1 = static_cast<short>(100.f * c / y_res_);
  if (n0 != n1)
    (std::cout << n1 << ' ').flush();
}

// dir points towards objects
glm::vec3
scene::sample_pixel(glm::vec3& pos, glm::vec3& dir, unsigned int depth,
                    unsigned short* nb) const
{
  glm::vec3 color(0.f, 0.f, 0.f);
  voxel v = intersect_ray(pos, dir);
  // If nothing was intersected return black
  if (!v.mat)
    return (color);

  // reflective vector wrt dir
  glm::vec3 reflect = glm::normalize(dir - 2.f * glm::dot(v.norm, dir) *
                                     v.norm);
  color += v.mat->get_emissive();
  if (depth)
    {
      // Check if object is specular
      if (basic_ray_tracing && glm::length(v.mat->get_specular()) > 0.0001f)
        {
          // Offset pos slightly to avoid numerical errors otherwise we might
          // intersect with ourself
          // Compute reflected color
          auto p = v.pos + 0.01f * reflect;
          color += v.mat->get_specular() * sample_pixel(p, reflect, depth - 1, nb);
        }
      else if (!basic_ray_tracing)
        {
          auto p = v.pos + 0.01f * reflect;
          glm::vec3 rand_dir = create_rand_dir(reflect, nb);
          color += v.mat->get_diffuse() * sample_pixel(p, rand_dir, depth - 1, nb);
        }
    }
  // Uncomment this to get basic ray tracing
  if (basic_ray_tracing)
    compute_light(v, color, reflect);


  return color;
}

// Cosine weighted distirbution along norm direction
glm::vec3 scene::create_rand_dir(glm::vec3& norm, unsigned short* nb) const
{
  // Diffuse sampling
  float cos_theta_square = static_cast<float>(erand48(nb));
  float cos_theta = sqrtf(cos_theta_square);
  float sin_theta = sqrtf(1.f - cos_theta_square);

  float phi = 2.f * static_cast<float>(M_PI * erand48(nb));
  float sin_phi = glm::sin(phi);
  float cos_phi = glm::cos(phi);

  glm::vec3 z = glm::normalize(norm);
  glm::vec3 y = glm::normalize(glm::vec3(z.z - z.y, z.x, -z.x));
  glm::vec3 x = glm::normalize(glm::cross(y, z));

  glm::vec3 rand_dir = sin_theta * cos_phi * x +
                       sin_theta * sin_phi * y +
                       cos_theta * z;
  rand_dir = glm::normalize(rand_dir);
  return rand_dir;
}

void
scene::compute_light(voxel& v, glm::vec3& color, glm::vec3& reflect) const
{
  for (const auto& l: dir_lights_)
    {
      // Cheat since inderect lighting is not handled
      color += v.mat->get_ambient() * l->get_ambient();

      glm::vec3 light_dir = glm::normalize(l->get_position());

      auto p = v.pos + 10.f * l->get_position();
      auto d = -light_dir;
      voxel c = intersect_ray(p, d);
      if (glm::distance(c.pos,v.pos) > 0.01f)
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
