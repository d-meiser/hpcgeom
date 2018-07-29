#include <test_utilities.h>
#include <random>
#include <basic_types.h>
#include <hashed_octree.h>


namespace {
std::random_device rd;
std::mt19937 gen(rd());
}

#ifdef _WIN32
#include <intrin.h>
uint64_t rdtsc(){
    return __rdtsc();
}
#else
uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
#endif

void BuildVerticesAtRandomLocations(struct GeoVertexArray *va,
                                    const struct GeoBoundingBox* bbox,
                                    int n, int *indices) {
  std::uniform_real_distribution<> dist_x(bbox->min.x, bbox->max.x);
  std::uniform_real_distribution<> dist_y(bbox->min.y, bbox->max.y);
  std::uniform_real_distribution<> dist_z(bbox->min.z, bbox->max.z);
  for (int i = 0; i < n; ++i) {
    va->x[i] = dist_x(gen);
    va->y[i] = dist_y(gen);
    va->z[i] = dist_z(gen);
    va->ptrs[i] = indices + i;
  }
}

void FillWithRandomItems(struct GeoVertexArray *va, struct GeoBoundingBox *bbox,
                         int n, int *indices) {
  for (int i = 0; i < n; ++i) {
    indices[i] = i;
  }
  BuildVerticesAtRandomLocations(va, bbox, n, indices);
}

struct GeoBoundingBox UnitCube() {
  return {{0, 0, 0,}, {1, 1, 1}};
}

#define MY_CONDITIONAL_SWAP(a, b) if ((a) > (b)) { tmp = (a); (a) = (b); (b) = tmp; }
void fix_box(struct GeoBoundingBox *b) {
  double tmp;
  MY_CONDITIONAL_SWAP(b->min.x, b->max.x)
  MY_CONDITIONAL_SWAP(b->min.y, b->max.y)
  MY_CONDITIONAL_SWAP(b->min.z, b->max.z)
}

void FillWithRandomVolumes(struct GeoBoundingBox *boxes,
    void **data, int n, const struct GeoBoundingBox *bbox, int *indices) {
  for (int i = 0; i < n; ++i) {
    indices[i] = i;
  }
  std::uniform_real_distribution<> dist_x(bbox->min.x, bbox->max.x);
  std::uniform_real_distribution<> dist_y(bbox->min.y, bbox->max.y);
  std::uniform_real_distribution<> dist_z(bbox->min.z, bbox->max.z);
  for (int i = 0; i < n; ++i) {
    boxes[i].min.x = dist_x(gen);
    boxes[i].min.y = dist_y(gen);
    boxes[i].min.z = dist_z(gen);
    boxes[i].max.x = dist_x(gen);
    boxes[i].max.y = dist_y(gen);
    boxes[i].max.z = dist_z(gen);
    fix_box(&boxes[i]);
    data[i] = &indices[i];
  }
}


