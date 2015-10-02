#ifndef RENDER_SCENE_CPP
# define RENDER_SCENE_CPP

# include <vector>

# include <glm/glm.hpp>
# include <glm/gtc/type_ptr.hpp> /* value_ptr */

# include <memory>

# include "obj_parser/object.hpp"
# include "camera/camera.hpp"
# include "light/light.hpp"
# include "extra/polygon.hpp"

class scene
{
public:
    using pos_lights = std::vector<pos_light*>;
    using dir_lights = std::vector<dir_light*>;
    using spot_lights = std::vector<spot_light*>;
    using objects = std::vector<object*>;

    scene(boundary& bound, unsigned x_res, unsigned y_res);
    void add_object(object *object);
    template <class light_t>
    void add_light(light_t* light);
    // This does a ray tracing on all objects and print result in  out.ppm
    void render();
    void dump_to_file(std::vector<glm::vec3>& vect);

private:
    camera camera_;

    objects objects_;

    dir_lights dir_lights_;
    pos_lights pos_lights_;
    spot_lights spot_lights_;

    // width of scene in world size
    float width_;
    // height of scene in world size
    float height_;
    // resolution of image along x and y axis (number of pixels)
    unsigned x_res_;
    unsigned y_res_;
};

# include "scene/scene.hxx"

#endif // RENDER_SCENE_CPP