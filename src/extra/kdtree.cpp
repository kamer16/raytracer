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
  box_ = { { pos.x - width / 2, pos.y - height / 2, pos.z - depth / 2 },
          { pos.x + width / 2, pos.y + height / 2, pos.z + depth / 2 } };
}

kdtree::kdtree()
  : left_(nullptr), right_(nullptr)
{
  box_ = { { FLT_MAX, FLT_MAX, FLT_MAX },
          { -FLT_MAX, -FLT_MAX, -FLT_MAX } };
}

void kdtree::compute_box(const std::vector<unsigned int>& indices,
                         const std::vector<utility::vertex_vnta>& vertices)
{
  for (unsigned i = 0; i < indices.size(); ++i)
    {
      const glm::vec3& v = vertices[indices[i]].v;
      box_.min_.x = std::min(v.x - 0.001f, box_.min_.x);
      box_.min_.y = std::min(v.y - 0.001f, box_.min_.y);
      box_.min_.z = std::min(v.z - 0.001f, box_.min_.z);

      box_.max_.x = std::max(v.x + 0.001f, box_.max_.x);
      box_.max_.y = std::max(v.y + 0.001f, box_.max_.y);
      box_.max_.z = std::max(v.z + 0.001f, box_.max_.z);
    }
}



bool kdtree::split(const std::vector<unsigned int>& indices,
                   const std::vector<utility::vertex_vnta>& vertices)
{
  glm::vec3 diff = box_.max_ - box_.min_;
  unsigned char axis = diff.x > diff.y && diff.x > diff.z ? 0 :
                      (diff.y > diff.z ? 1 : 2);
  (void) axis;
  (void) indices;
  // Split along axis
  std::vector<unsigned int> left_bucket;
  std::vector<unsigned int> right_bucket;

  float cut = (box_.max_[axis] + box_.min_[axis]) / 2;
  for (unsigned int i = 0; i < indices.size(); i += 3)
    {
      const glm::vec3& v0 = vertices[indices[i]].v;
      const glm::vec3& v1 = vertices[indices[i + 1]].v;
      const glm::vec3& v2 = vertices[indices[i + 2]].v;
      if (v0[axis] < cut || v1[axis] < cut || v2[axis] < cut)
        {
          left_bucket.push_back(indices[i]);
          left_bucket.push_back(indices[i + 1]);
          left_bucket.push_back(indices[i + 2]);
        }
      else
        {
          right_bucket.push_back(indices[i]);
          right_bucket.push_back(indices[i + 1]);
          right_bucket.push_back(indices[i + 2]);
        }
    }
  if (left_bucket.size() == indices.size())
    return false;
  assert(left_bucket.size() < indices.size() && right_bucket.size() < indices.size());
  left_ = create_kdtree(left_bucket, vertices);
  right_ = create_kdtree(right_bucket, vertices);
  return true;
}

kdtree* create_kdtree(const std::vector<unsigned int>& indices,
                      const std::vector<utility::vertex_vnta>& vertices)
{
  kdtree* res;
  unsigned max_elt = 32;
  if (indices.size() < max_elt)
    {
      res = new kdtree_leaf;
      res->compute_box(indices, vertices);
      kdtree_leaf* leaf = static_cast<kdtree_leaf*>(res);
      // TODO this is a huge copy, should find a way to only have indices
      // duplicated between material and kdtree
      leaf->indices_ = indices;
    }
  else
    {
      res = new kdtree;
      res->compute_box(indices, vertices);
      if (!res->split(indices, vertices))
        {
          delete res;
          res = new kdtree_leaf;
          res->compute_box(indices, vertices);
          kdtree_leaf* leaf = static_cast<kdtree_leaf*>(res);
          // TODO this is a huge copy, should find a way to only have indices
          // duplicated between material and kdtree
          leaf->indices_ = indices;
        }
    }
  return res;
}
