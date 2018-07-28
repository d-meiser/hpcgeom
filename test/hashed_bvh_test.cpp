#include <gtest/gtest.h>
#include <hashed_bvh.h>


TEST(HashedBvh, Initialize) {
  struct GeoHashedBvh bvh;
  GeoHBInitialize(&bvh, {{0, 0, 0}, {1, 1, 1}});
  GeoHBDestroy(&bvh);
}

