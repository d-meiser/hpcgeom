#include <gtest/gtest.h>
#include <hashed_bvh.h>


TEST(HashedBvh, Initialize) {
  struct GeoHashedBvh bvh;
  GeoHBInitialize(&bvh, {{0, 0, 0}, {1, 1, 1}});
  GeoHBDestroy(&bvh);
}

TEST(HashedBvh, InsertSomeVolumes) {
  struct GeoHashedBvh bvh;
  GeoHBInitialize(&bvh, {{0, 0, 0}, {1, 2, 3}});
  int n = 10;
  std::vector<struct GeoBoundingBox> volumes(n,
      {{0.2, 0.3, 0.4}, {0.3, 0.4, 0.7}});
  std::vector<void*> data(n);
  GeoHBInsert(&bvh, n, &volumes[0], &data[0]);
  GeoHBDestroy(&bvh);
}

