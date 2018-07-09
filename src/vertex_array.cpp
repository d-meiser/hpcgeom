#include <vertex_array.h>
#include <cstdlib>
#include <cstdint>


static_assert(sizeof(double) == 8, "Size of double must be 8.");
static_assert(sizeof(void*) == 8, "Size of void* must be 8.");
static_assert(hpcgeo::VertexArray::ALIGNMENT % sizeof(double) == 0,
              "VertexArray::ALIGNMENT must be a multiple of sizeof(double)");


namespace hpcgeo {

namespace {

int required_storage(int capacity, int alignment) {
  return (capacity / alignment + 1) * alignment * 4;
}

template<typename T>
T* make_aligned(T* ptr, int alignment) {
  uint64_t p = reinterpret_cast<uint64_t>(ptr);
  int misalignment = p % alignment;
  if (misalignment) {
    p = p + (alignment - misalignment);
  }
  return reinterpret_cast<T*>(p);
}

}

VertexArray::VertexArray() : size_(0) {
  capacity_ = 16;
  data_ = malloc(required_storage(capacity_, ALIGNMENT));
  x_ = static_cast<double*>(data_);
  x_ = make_aligned(x_, ALIGNMENT);
  y_ = x_ + capacity_;
  y_ = make_aligned(y_, ALIGNMENT);
  z_ = z_ + capacity_;
  z_ = make_aligned(z_, ALIGNMENT);
  ptrs_ = reinterpret_cast<void**>(z_) + capacity_;
  ptrs_ = make_aligned(ptrs_, ALIGNMENT);
}

VertexArray::~VertexArray() {
  free(data_);
}

double* VertexArray::x() {
  return x_;
}

double* VertexArray::y() {
  return y_;
}

double* VertexArray::z() {
  return z_;
}

void** VertexArray::ptrs() {
  return ptrs_;
}

int VertexArray::size() const {
  return size_;
}

}
