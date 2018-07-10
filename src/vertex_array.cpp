#include <vertex_array.h>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>


static_assert(sizeof(double) == 8, "Size of double must be 8.");
static_assert(sizeof(void*) == 8, "Size of void* must be 8.");
#define CHUNK_SIZE 8
static_assert(
    hpcgeo::VertexArray::ALIGNMENT % (CHUNK_SIZE * sizeof(double)) == 0,
    "VertexArray::ALIGNMENT must be a multiple of CHUNK_SIZE * sizeof(double)");


namespace hpcgeo {

namespace {

int required_storage(int capacity, int alignment) {
  return (capacity * sizeof(double) / alignment + 1) * alignment * 4;
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
  *z = *y + capacity;
  *z = make_aligned(*z, alignment);
  *ptrs = reinterpret_cast<void**>(*z) + capacity;
  *ptrs = make_aligned(*ptrs, alignment);
}

template<typename T>
void aligned_chunked_copy(T* dest, const T* src, int n) {
  static_assert(sizeof(T) == 8,
                "aligned_chunked_copy assumes type of size 8 bytes.");
  assert(n % CHUNK_SIZE == 0);
  int num_chunks = n / CHUNK_SIZE;
  T*__restrict__ x =
      (T*)__builtin_assume_aligned(dest, VertexArray::ALIGNMENT);
  const T*__restrict__ y =
      (const T*)__builtin_assume_aligned(src, VertexArray::ALIGNMENT);
  for (int i = 0; i < num_chunks; ++i) {
    for (int j = 0; j < CHUNK_SIZE; ++j) {
      x[i * CHUNK_SIZE + j] = y[i * CHUNK_SIZE + j];
    }
  }
}

} // anonymous namespace

VertexArray::VertexArray() : size_(0) {
  capacity_ = 16;
  data_ = malloc(required_storage(capacity_, ALIGNMENT));
  set_ptrs(data_, capacity_, ALIGNMENT, &x_, &y_, &z_, &ptrs_);
}

VertexArray::VertexArray(const VertexArray& other) : size_(0), capacity_(0),
    data_(nullptr), x_(nullptr), y_(nullptr), z_(nullptr), ptrs_(nullptr) {
  resize(other.size_);
  aligned_chunked_copy(x_, other.x_, capacity_);
  aligned_chunked_copy(y_, other.y_, capacity_);
  aligned_chunked_copy(z_, other.z_, capacity_);
  aligned_chunked_copy(ptrs_, other.ptrs_, capacity_);
}

VertexArray::VertexArray(VertexArray&& other) :
    size_(other.size_), capacity_(other.capacity_),
    data_(other.data_), x_(other.x_), y_(other.y_),
    z_(other.z_), ptrs_(other.ptrs_) {
  other.size_ = 0;
  other.capacity_ = 0;
  other.data_ = nullptr;
  other.x_ = nullptr;
  other.y_ = nullptr;
  other.z_ = nullptr;
  other.ptrs_ = nullptr;
}

VertexArray& VertexArray::operator=(const VertexArray& rhs) {
  if (this != &rhs) {
    free(data_);
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
    resize(rhs.size_);
    aligned_chunked_copy(x_, rhs.x_, capacity_);
    aligned_chunked_copy(y_, rhs.y_, capacity_);
    aligned_chunked_copy(z_, rhs.z_, capacity_);
    aligned_chunked_copy(ptrs_, rhs.ptrs_, capacity_);
  }
  return *this;
}

VertexArray& VertexArray::operator=(VertexArray&& rhs) {
  if (this != &rhs) {
    size_ = rhs.size_;
    capacity_ = rhs.capacity_;
    free(data_);
    data_ = rhs.data_;
    x_ = rhs.x_;
    y_ = rhs.y_;
    z_ = rhs.z_;
    ptrs_ = rhs.ptrs_;
    rhs.size_ = 0;
    rhs.capacity_ = 0;
    rhs.data_ = nullptr;
    rhs.x_ = nullptr;
    rhs.y_ = nullptr;
    rhs.z_ = nullptr;
    rhs.ptrs_ = nullptr;
  }
  return *this;
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
    asm("# Copying old arrays to new arrays");
    aligned_chunked_copy(new_x, x_, capacity_);
    aligned_chunked_copy(new_y, y_, capacity_);
    aligned_chunked_copy(new_z, z_, capacity_);
    aligned_chunked_copy(new_ptrs, ptrs_, capacity_);
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
