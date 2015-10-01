#include "obj_parser/obj_loader.hpp"

int main(int argc, char* argv[])
{
  if (argc <= 1)
    return 1;
  std::string obj_file(argv[1]);

  obj_loader loader;
  object* obj = loader.load_obj(obj_file);
  delete obj;
}
