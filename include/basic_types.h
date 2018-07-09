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

class VertexArray {
 public:
  double* x();
  double* y();
  double* z();
  void** ptrs();

  int size() const;
  void reserve(int size);
  void resize(int size);

 private:
  int capacity_;
  int size_;
  void* data_;
};

} // namespace hpcgeo

#endif

