template<class material>
float
intersect_ray(material& mat, glm::vec3& eye, glm::vec3& look_at, glm::vec3& color)
{
  using vertex_t = typename material::value_type;
  const std::vector<vertex_t>& vertices = mat.get_vertices();
  const auto& indices = mat.get_indices();
  float min_dist = FLT_MAX;
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
      // normals generates up ta 5% mistakes due to lack of float precision
      glm::vec3 n = glm::normalize(glm::cross(u, v));
      glm::vec3 eye_dir = glm::normalize(eye - look_at);
      float dot_val = glm::dot(n, glm::normalize(eye_dir));
      // Ignore when ray is parallel to triangle or wrong side
      if (dot_val <= std::numeric_limits<float>::epsilon())
        continue;
      // Find intersection on plane
      float r = glm::dot(n, eye - v0) / glm::dot(n, eye_dir);
      if (r <= 0.0f)
        continue;

      // The intersecting point
      glm::vec3 p = eye - r * (eye_dir);
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
          // Light projects (0.2f, 0.8f, 0.8f) of (ambient, diffuse, specular)
          glm::vec3 interpolated_normal = glm::normalize(s * n1 + t * n2 +
                                                         (1 - (t + s)) * n0);
          // Choose an arbitrary light direction, should use light class (TODO)
          glm::vec3 light_dir = glm::normalize(glm::vec3(0.3f, 0.3f, 0.3f));
          dot_val = glm::dot(interpolated_normal, light_dir);
          float new_dist = glm::distance(p, eye);
          if (new_dist < min_dist)
            {
              glm::vec3 reflect = glm::normalize(-light_dir + 2.f *
                                  glm::dot(interpolated_normal, light_dir) *
                                  interpolated_normal);
              float dot_reflect = std::max(0.f, glm::dot(reflect, eye_dir));
              color = mat.get_ambient() * 0.2f * 255.f;
              if (dot_val > 0.0f)
                color += (dot_val * mat.get_diffuse() * 0.8f+
                          powf(dot_reflect, mat.get_shininess()) * 0.8f *
                          mat.get_specular()) * 255.f;
              min_dist =  new_dist;
            }
        }
    }
  color.x = std::min(255.f, color.x);
  color.y = std::min(255.f, color.y);
  color.z = std::min(255.f, color.z);

  return min_dist;
}

// Temporary function
template<>
inline float
intersect_ray<material_v>(material_v& mat, glm::vec3& eye, glm::vec3& look_at,
                          glm::vec3& color)
{
  assert(0 && "This function should not be called and will be removed in future");
  exit(1);
  using vertex_t = typename material_v::value_type;
  const std::vector<vertex_t> vertices = mat.get_vertices();
  auto& indices = mat.get_indices();
  float min_dist = FLT_MAX;
  for (unsigned i = 0; i < indices.size(); i += 3)
    {
      const glm::vec3& v0 = vertices[indices[i]].v;
      const glm::vec3& v1 = vertices[indices[i + 1]].v;
      const glm::vec3& v2 = vertices[indices[i + 2]].v;
      // compute normal of triangle to determine it's plane
      glm::vec3 u = v1 - v0;
      glm::vec3 v = v2 - v0;
      // Computing cross seems faster than averaging 3 normals
      glm::vec3 n = glm::cross(u, v);
      float dot_val = fabsf(glm::dot(glm::normalize(n), glm::normalize(look_at - eye)));
      // Ignore when ray is parallel to triangle
      if (dot_val <= std::numeric_limits<float>::epsilon())
        continue;
      // Find intersection on plane
      float r = glm::dot(n, v0 - eye) / glm::dot(n, look_at - eye);
      if (r <= 0.0f)
        continue;

      // The intersecting point
      glm::vec3 p = eye + r * (look_at - eye);
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
          float new_dist = glm::distance(p, eye);
          if (new_dist < min_dist)
            {
              color = dot_val * glm::vec3((mat.get_ambient() + mat.get_diffuse()) *
                                255.0f / 2.0f);
              min_dist =  new_dist;
            }
        }
    }

  return min_dist;
}



template<class material>
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
