#ifndef POLYGONS_HPP
# define POLYGONS_HPP

# include <array>

# include "obj_parser/object.hpp"
# include "extra/utility.hpp"

namespace polygon
{

// Object of Three lines representing X,Y,Z axis
object* make_coordinate();
object* make_quad_xz();
object* make_quad_xy();
object* make_cube();
object* make_sphere(unsigned stacks, unsigned slices, float radius = 1);

}

#endif // POLYGONS_HPP
