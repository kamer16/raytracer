template<class material>
float
intersect_ray(material& mat, glm::vec3& L, glm::vec3& look_at)
{
  using vertex_t = typename material::value_type;
  const std::vector<vertex_t> vertices = mat.get_vertices();
  auto& indices = mat.get_indices();
  for (unsigned i = 0; i < indices.size(); i += 3)
    {
      glm::vec3 v0 = vertices[indices[i]].v;
      glm::vec3 v1 = vertices[indices[i + 1]].v;
      glm::vec3 v2 = vertices[indices[i + 2]].v;
      // compute normal of triangle to determine it's plane
      glm::vec3 u = v1 - v0;
      glm::vec3 v = v2 - v0;
      glm::vec3 n = glm::cross(u, v);

      // Ignore when ray is parallel to triangle
      if (fabs(glm::dot(n, look_at - L)) <= std::numeric_limits<float>::epsilon())
        continue;
      // Find intersection on plane
      float r = glm::dot(n, v0 - L) / glm::dot(n, look_at - L);
      if (r <= 0.0f)
        continue;

      // The intersecting point
      glm::vec3 p = L + r * (look_at - L);
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
        return 1;
    }

  return -1;
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
