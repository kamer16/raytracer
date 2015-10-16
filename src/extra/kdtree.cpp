#include <algorithm>

#include "kdtree.hpp"

bool
aabb::intersect(glm::vec3& pos, glm::vec3& inv_dir) {
  using namespace std;
  float tx1 = (min_.x - pos.x)*inv_dir.x;
  float tx2 = (max_.x - pos.x)*inv_dir.x;

  float tmin = min(tx1, tx2);
  float tmax = max(tx1, tx2);

  float ty1 = (min_.y - pos.y) * inv_dir.y;
  float ty2 = (max_.y - pos.y) * inv_dir.y;

  tmin = max(tmin, min(ty1, ty2));
  tmax = min(tmax, max(ty1, ty2));

  float tz1 = (min_.z - pos.z) * inv_dir.z;
  float tz2 = (max_.z - pos.z) * inv_dir.z;

  tmin = max(tmin, min(tz1, tz2));
  tmax = min(tmax, max(tz1, tz2));

  return tmax >= 0 && tmax >= tmin;
}

kdtree::kdtree(float width, float height, float depth, glm::vec3& pos)
  : left_(nullptr), right_(nullptr)
{
  box = { { pos.x - width / 2, pos.y - height / 2, pos.z - depth / 2 },
          { pos.x + width / 2, pos.y + height / 2, pos.z + depth / 2 } };
}

kdtree::kdtree()
  : left_(nullptr), right_(nullptr)
{
  box = { { FLT_MAX, FLT_MAX, FLT_MAX },
          { -FLT_MAX, -FLT_MAX, -FLT_MAX } };
}
