#include <vertex_array.h>
#include <cstdlib>
#include <cstdint>
#include <cstring>


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

void set_ptrs(void* d, int capacity, int alignment,
              double** x, double** y, double** z, void*** ptrs) {
  *x = static_cast<double*>(d);
  *x = make_aligned(*x, alignment);
  *y = *x + capacity;
  *y = make_aligned(*y, alignment);
  *z = *z + capacity;
  *z = make_aligned(*z, alignment);
  *ptrs = reinterpret_cast<void**>(*z) + capacity;
  *ptrs = make_aligned(*ptrs, alignment);
}

}

VertexArray::VertexArray() : size_(0) {
  capacity_ = 16;
  data_ = malloc(required_storage(capacity_, ALIGNMENT));
  set_ptrs(data_, capacity_, ALIGNMENT, &x_, &y_, &z_, &ptrs_);
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

void VertexArray::resize(int size) {
  if (size > capacity_) {
    static constexpr int dalign = ALIGNMENT / sizeof(double);
    int new_capacity = (size / dalign + 1) * dalign;
    void* new_data = malloc(required_storage(new_capacity, ALIGNMENT));
    double* new_x;
    double* new_y;
    double* new_z;
    void** new_ptrs;
    set_ptrs(new_data, new_capacity, ALIGNMENT,
             &new_x, &new_y, &new_z, &new_ptrs);
    memcpy(new_x, x_, size_ * sizeof(*x_));
    memcpy(new_y, y_, size_ * sizeof(*y_));
    memcpy(new_z, z_, size_ * sizeof(*z_));
    memcpy(new_ptrs, ptrs_, size_ * sizeof(*ptrs_));
    free(data_);
    data_ = new_data;
    x_ = new_x;
    y_ = new_y;
    z_ = new_z;
    ptrs_ = new_ptrs;
    capacity_ = new_capacity;
  }
  size_ = size;
}

}
