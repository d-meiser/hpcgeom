#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H


namespace hpcgeo {

class VertexArray {
 public:
  VertexArray();
  VertexArray(const VertexArray& other);
  VertexArray(const VertexArray&& other);
  VertexArray& operator=(const VertexArray& rhs);
  VertexArray& operator=(const VertexArray&& rhs);
  ~VertexArray();
  double* x();
  double* y();
  double* z();
  void** ptrs();

  int size() const;
  void resize(int size);

  static constexpr unsigned int ALIGNMENT = 128;

 private:
  int capacity_;
  int size_;
  void* data_;
  double* x_;
  double* y_;
  double* z_;
  void** ptrs_;
};

} // namespace hpcgeo

#endif
