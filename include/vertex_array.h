#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H


namespace hpcgeo {

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
