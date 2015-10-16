#ifndef RENDER_SCENE_CPP
# define RENDER_SCENE_CPP

# include <vector>
# include <atomic>

# include <glm/glm.hpp>
# include <glm/gtc/type_ptr.hpp> /* value_ptr */
#include <tbb/blocked_range.h>

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

    scene(boundary& bound, unsigned int x_res, unsigned int y_res);
    void add_object(object *object);
    template <class light_t>
    void add_light(light_t* light);
    // This does a ray tracing on all objects and print result in  out.ppm
    void render();
    // Used by tbb::parallel_for
    void operator() (const tbb::blocked_range<unsigned int>& r) const;

private:
    glm::vec3 sample_pixel(glm::vec3& eye, glm::vec3& ray, unsigned int depth,
                           unsigned short* nm, float Ni) const;
    void compute_light(voxel& v, glm::vec3& color, glm::vec3& reflect) const;
    void dump_to_file() const;
    // Can be used to create new random normal for the monte carlo method
    // TODO Check if the new normal is in hemisphere
    glm::vec3 create_rand_dir(glm::vec3& norm, unsigned short* nb) const;
    voxel intersect_ray(glm::vec3& pos, glm::vec3& dir) const;

    float reflectance(const glm::vec3& normal, const glm::vec3& incident,
                      float n1, float n2) const;
    glm::vec3 refract(const glm::vec3& normal, const glm::vec3& incident,
                      float n1, float n2) const;
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
    unsigned int x_res_;
    unsigned int y_res_;
    std::vector<glm::vec3> res_;
    static std::atomic<unsigned short> counter;
    bool basic_ray_tracing = false;
};

# include "scene/scene.hxx"

#endif // RENDER_SCENE_CPP
