# include <glm/gtc/matrix_transform.hpp> /* lookAt, perspective */

#include "camera/camera.hpp"

camera::camera(float aspect_ratio)
    : position_(0, 1, 1.0),
      rotation_(-0.4, -3.14),
      aspect_ratio_(aspect_ratio),
      fov_(65.0f)
{
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
    view_mat_ = glm::lookAt(position_, position_ + forward_,
                            glm::vec3(0, 1, 0));
    return view_mat_;
}


