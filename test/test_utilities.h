#ifndef TEST_UTILITIES_H
#define TEST_UTILITIES_H

#include <cstdint>


uint64_t rdtsc();
void FillWithRandomItems(struct GeoVertexArray *va, struct GeoBoundingBox *bbox,
    int n, int *indices);
void FillWithRandomVolumes(struct GeoBoundingBox *boxes, void **data,
    int n, const struct GeoBoundingBox *bbox, int *indices);
struct GeoBoundingBox UnitCube();

#endif
