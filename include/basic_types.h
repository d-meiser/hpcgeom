#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H


namespace hpcgeo {

struct Point {
  double x;
  double y;
  double z;
};

struct BoundingBox {
  Point min;
  Point max;
};

struct Vertex {
  Point p;
  void* ptr;
};

} // namespace hpcgeo

#endif

