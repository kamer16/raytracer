#include <tbb/task_scheduler_init.h>

#include "obj_parser/obj_loader.hpp"
#include "scene/scene.hpp"

static unsigned int get_arg(int argc, char* argv[], int index, unsigned int def)
{
    int res = 0;
    if (argc > index)
      res = atoi(argv[index]);
    if (res > 0 && res < 10000)
      return static_cast<unsigned int>(res);
    return def;
}

int main(int argc, char* argv[])
{
  if (argc <= 1)
    return 1;
  tbb::task_scheduler_init init;
  std::string obj_file(argv[1]);

  obj_loader loader;
  object* obj = loader.load_obj(obj_file);
  auto boundary = obj->get_boundary();
  unsigned int y_res = get_arg(argc, argv, 2, 300);
  unsigned int x_res = static_cast<unsigned int>((boundary.width / boundary.height) * y_res);
  scene::nb_samples = get_arg(argc, argv, 3, 1000);
  scene scenery(boundary, x_res, y_res);
  dir_light* d_light = light_dir_default_new();
  pos_light* p_light = light_pos_default_new();

  scenery.add_object(obj);
  scenery.add_light(p_light);

  // This does a ray tracing on all objects and print result in  out.ppm
  scenery.render();

  delete obj;
  delete d_light;
  delete p_light;
}
