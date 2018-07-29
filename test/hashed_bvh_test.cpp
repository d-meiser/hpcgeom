#include <gtest/gtest.h>
#include <hashed_bvh.h>


struct HashedBvh : public ::testing::Test {
  struct GeoHashedBvh bvh;
  void SetUp() override {
    GeoHBInitialize(&bvh, {{0.2, 1.3, -5.2}, {4.0, 2.5, 1.0}});
  }
  void TearDown() override {
    GeoHBDestroy(&bvh);
  }
};

TEST_F(HashedBvh, Initialize) {}

TEST_F(HashedBvh, InsertSomeVolumes) {
  int n = 10;
  std::vector<struct GeoBoundingBox> volumes(n,
      {{0.2, 0.3, 0.4}, {0.3, 0.4, 0.7}});
  std::vector<void*> data(n);
  GeoHBInsert(&bvh, n, &volumes[0], &data[0]);
}

