template<class material>
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
      float dot_val = glm::dot(n, glm::normalize(eye_dir));
      // Ignore when ray is parallel to triangle or wrong side
      if (dot_val <= std::numeric_limits<float>::epsilon())
        continue;
      // Find intersection on plane
      float r = glm::dot(n, eye_pos - v0) / glm::dot(n, eye_dir);
      if (r <= 0.0f)
        continue;

      // The intersecting point
      glm::vec3 p = eye_pos - r * (eye_dir);
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
          if (new_dist < res.dist)
            {
              res.dist = new_dist;
              res.norm = interpolated_normal;
              res.pos = v0 + s * v1 + t * v2;
              res.mat = &mat;
            }
        }
    }

  return res;
}

// Temporary function
template<>
inline voxel
intersect_ray<material_v>(material_v& mat, glm::vec3& eye, glm::vec3& look_at)
{
  (void) mat;
  (void) eye;
  (void) look_at;
  assert(0 && "This function should not be called and will be removed in future");
  exit(1);
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
