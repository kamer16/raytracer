#include <glm/gtc/matrix_transform.hpp>

#include "light/light.hpp"

dir_light* light_dir_default_new()
{
    dir_light* res = new dir_light;
    glm::vec3 light_color(0.0f, 0.0f, 1.0);
    glm::vec3 ambient(0.0f, 0.0f, 0.0f);
    glm::vec3 diffuse(light_color);
    glm::vec3 specular(light_color);
    glm::vec3 pos(0.0f, 1.0f, 0.1f);
    res->set_ambient(ambient);
    res->set_diffuse(diffuse);
    res->set_specular(specular);
    res->set_position((pos));
    return res;
}

pos_light* light_pos_default_new()
{
    pos_light* res = new pos_light;
    glm::vec3 light_color(0.0f, 1.0f, 0.0);
    glm::vec3 ambient(0.0f, 0.0f, 0.0f);
    glm::vec3 diffuse(light_color);
    glm::vec3 specular(light_color);
    glm::vec3 pos(80.0f, 10.0f, 0.0f);
    res->set_ambient(ambient);
    res->set_diffuse(diffuse);
    res->set_specular(specular);
    res->set_position(pos);
    return res;
}

spot_light* light_spot_default_new()
{
    spot_light* res = new spot_light;
    glm::vec3 light_color(1.0f, 0.0f, 0.0);
    glm::vec3 ambient(0.0f, 0.0f, 0.0f);
    glm::vec3 diffuse(light_color);
    glm::vec3 specular(light_color);
    glm::vec3 pos(0.0f, 25.0f, 80.0f);
    res->set_ambient(ambient);
    res->set_diffuse(diffuse);
    res->set_specular(specular);
    res->set_position(pos);
    return res;
}

float
pos_light::get_scale()
{
    float c = 0;
    for (int i = 0; i < 3; ++i)
        c = diffuse_[i] < c ? c : diffuse_[i];
    float dist = -linear_att_;
    dist += sqrtf(linear_att_ * linear_att_ - 4 * quadratic_att_ *
                  (const_att_ - 256 * c));
    dist /=  2 * quadratic_att_;
    return dist;
}
