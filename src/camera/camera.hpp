#ifndef CAMERA_HPP
# define CAMERA_HPP

// GLM_FORCE_RADIANS needs to be defined before glm.hpp include to force radians
# define GLM_FORCE_RADIANS

# include <glm/glm.hpp>

class camera
{
public:
    camera(float aspect_ratio);
    const glm::mat4& get_proj_mat();
    const glm::mat4& get_view_mat();

private:
    glm::mat4 proj_mat_;
    glm::mat4 view_mat_;

    // Camera's world coordinate position
    glm::vec3 position_;
    // Camera's spherical coordinates i-e horizontal, vertical angles
    glm::vec2 rotation_;

    // The camera's z axis and direction being looked at
    glm::vec3 forward_;
    // the camera's x axis
    glm::vec3 right_;
    // the camera's y axis
    glm::vec3 up_;

    // windows width ratio to windows height
    float aspect_ratio_;
    // Field of view of camera which can simulate zooming
    float fov_;
};

#endif // CAMERA_HPP
