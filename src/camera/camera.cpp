# include <glm/gtc/matrix_transform.hpp> /* lookAt, perspective */

#include "camera/camera.hpp"
#include "extra/utility.hpp"

camera::camera(glm::vec3& look_at_z, float plane_dist)
    : position_(look_at_z + glm::vec3(0, 0, plane_dist)),
      rotation_(0.0, 0),
      fov_(65.0f),
      plane_dist_(plane_dist)
{
    // Spherical coordinates to cartesian coordinates formula
    backward_ = glm::vec3(glm::cos(rotation_.x) * glm::sin(rotation_.y),
                         glm::sin(rotation_.x),
                         glm::cos(rotation_.x) * glm::cos(rotation_.y));
    // Always on horizontal i-e y == 0
    // direction is the one of y_axis rotation + pi/2.
    right_ = glm::vec3(glm::sin(rotation_.y + 3.14f / 2.0f),
                       0,
                       glm::cos(rotation_.y + 3.14f / 2.0f));
    up_ = glm::cross(backward_, right_);

    backward_ = glm::normalize(backward_);
    right_ = glm::normalize(right_);
    up_ = glm::normalize(up_);
}

const glm::mat4&
camera::get_proj_mat()
{
    proj_mat_ = glm::perspective(glm::radians(fov_), aspect_ratio_,
                                 0.1f, 10000.0f);
    return proj_mat_;
}

const glm::mat4&
camera::get_view_mat()
{
    view_mat_ = glm::lookAt(position_, position_ + -backward_,
                            glm::vec3(0, 1, 0));
    return view_mat_;
}

glm::vec3
camera::get_pos_vec()
{
    return position_;
}

glm::vec3
camera::get_look_at()
{
    return position_ - backward_ * plane_dist_;
}

const glm::vec3&
camera::get_right()
{
    return right_;
}

const glm::vec3&
camera::get_up()
{
    return up_;
}
