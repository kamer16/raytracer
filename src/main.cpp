#include "obj_parser/obj_loader.hpp"
#include "scene/scene.hpp"

int main(int argc, char* argv[])
{
  if (argc <= 1)
    return 1;
  std::string obj_file(argv[1]);
  unsigned int x_res = 100;
  unsigned int y_res = 100;

  obj_loader loader;
  object* obj = loader.load_obj(obj_file);
  auto boundary = obj->get_boundary();
  scene scenery(boundary, x_res, y_res);
  pos_light* light = light_pos_default_new();

  scenery.add_object(obj);
  scenery.add_light(light);

  // This does a ray tracing on all objects and print result in  out.ppm
  scenery.render();

  delete obj;
  delete light;
}
