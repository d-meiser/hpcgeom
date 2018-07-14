#ifndef TEST_UTILITIES_H
#define TEST_UTILITIES_H

#include <cstdint>


uint64_t rdtsc();
void FillWithRandomItems(struct GeoVertexArray *va, struct GeoBoundingBox *bbox,
                     int n, int *indices);
struct GeoBoundingBox UnitCube();

#endif
