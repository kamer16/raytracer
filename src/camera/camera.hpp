#ifndef CAMERA_HPP
# define CAMERA_HPP

// GLM_FORCE_RADIANS needs to be defined before glm.hpp include to force radians
# define GLM_FORCE_RADIANS

# include <glm/glm.hpp>

class camera
{
public:
    // look_at_z is the best place to look when looking along -z axis
    // Bigger plane_dist gives more orthographic projection
    camera(glm::vec3& look_at_z, float plane_dist = 0.5f);
    // Used for OpenGL might need it in future
    const glm::mat4& get_proj_mat();
    // Used for OpenGL might need it in future
    const glm::mat4& get_view_mat();
    // Return the coordonate of camera
    glm::vec3 get_pos_vec();
    glm::vec3 get_look_at();
    const glm::vec3& get_right();
    const glm::vec3& get_up();

private:
    glm::mat4 proj_mat_;
    glm::mat4 view_mat_;

    // Camera's world coordinate position
    glm::vec3 position_;
    // Camera's spherical coordinates i-e vertical horizontal angles
    // Vertical (rotation along x axis) 0 -> pi
    // Horizontal (rotation along y axis) 0 -> 2pi
    glm::vec2 rotation_;

    // We use right hand coordinate system, x is right, y is up, z is back
    // The camera's z axis and opposite direction being looked at
    glm::vec3 backward_;
    // the camera's x axis
    glm::vec3 right_;
    // the camera's y axis
    glm::vec3 up_;

    // windows width ratio to windows height
    float aspect_ratio_;
    // Field of view of camera which can simulate zooming
    float fov_;
    // Used to know how far image plane is from eye.
    float plane_dist_;
};

#endif // CAMERA_HPP
