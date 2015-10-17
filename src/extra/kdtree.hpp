#pragma once

#include <glm/glm.hpp>

#include "extra/utility.hpp"
struct plane
{
  glm::vec3 norm;
  glm::vec3 point;
};

// Axis aligned bounding box
struct aabb
{
  // Define slabs which represents the six faces of cubes
  glm::vec3 min_;
  glm::vec3 max_;
  bool intersect(glm::vec3& pos, glm::vec3& inv_dir);
};

class kdtree
{
public:
  kdtree(float width, float height, float depth, glm::vec3& pos);
  kdtree();
  aabb box_;
  kdtree* left_ = nullptr;
  kdtree* right_ = nullptr;

  void compute_box(const std::vector<unsigned int>& indices,
                   const std::vector<utility::vertex_vnta>& vertices);
  bool split(const std::vector<unsigned int>& indices,
             const std::vector<utility::vertex_vnta>& vertices);
};

class kdtree_leaf : public kdtree
{
  using vertices_idx = std::vector<unsigned>;
public:
  // Associated indices to material, each pack of 3 indices represents a
  // triangle and the data is found in the vertex buffer whichi usually is on
  // the GPU in OpenGL
 vertices_idx indices_;
};

kdtree* create_kdtree(const std::vector<unsigned int>& indices,
                      const std::vector<utility::vertex_vnta>& vertices);
