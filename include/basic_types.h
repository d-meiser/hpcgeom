#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H


struct GeoPoint {
  double x;
  double y;
  double z;
};

struct GeoBoundingBox {
  struct GeoPoint min;
  struct GeoPoint max;
};

struct GeoVertex {
  struct GeoPoint p;
  void* ptr;
};

#endif

