#include "scene/scene.hpp"

scene::scene(float aspect_ratio)
    : camera_(aspect_ratio)
{
}

void
scene::add_object(object* object)
{
    objects_.push_back(object);
}
