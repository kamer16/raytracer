#ifndef OBJECT_HPP
# define OBJECT_HPP

# define GLM_FORCE_RADIANS

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp> /* lookAt, perspective */
# include <glm/gtx/transform.hpp> /* lookAt, perspective */
# include <glm/gtc/type_ptr.hpp> /* value_ptr */

# include <unordered_map>
# include <tuple>
# include <vector>

# include "extra/utility.hpp"
# include "obj_parser/material.hpp"

class object
{
public:
    using container3 = std::vector<glm::vec3>;
    using container2 = std::vector<glm::vec2>;
    using materials_t = std::vector<material*>;

    object() = default;
    ~object();
    void translate(const glm::vec3& dir);
    void scale(const glm::vec3& vec);
    void rotate(float degrees, const glm::vec3& dir);
    void set_model_mat(glm::mat4& model_mat);
    const glm::mat4& get_model_mat();
    void add_material(material* mat);
    // Can be accessed to be sorted
    materials_t& get_materials();
    // This functions sets the render_type of each materials associated to the
    // object to the new render_type
    void set_render_mode(render_type type);

    // Iterate on all vertices and computes width, height and best location
    // point.
    boundary get_boundary();
protected:
    materials_t materials_;
private:
    glm::mat4 model_mat_;
};

#endif // OBJECT_HPP
