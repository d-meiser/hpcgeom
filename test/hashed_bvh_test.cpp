#include <gtest/gtest.h>
#include <hashed_bvh.h>
#include <test_utilities.h>


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

extern "C" {
struct CountIntersectingVolumesCtx {
  int intersecting_volumes;
};

int CountTraversedNodes(struct GeoBoundingBox *volumes, void **data, int i,
                        void *ctx) {
  (void)volumes;
  (void)data;
  (void)i;
  struct CountIntersectingVolumesCtx* count_ctx =
      static_cast<CountIntersectingVolumesCtx*>(ctx);
  ++count_ctx->intersecting_volumes;
  return 1;
}

}

TEST_F(HashedBvh, VisitSmallVolumeIntersection) {
  int n = 10;
  std::vector<struct GeoBoundingBox> volumes(n,
      {{0.21, 1.4, 0.4}, {0.3, 1.8, 0.7}});
  std::vector<void*> data(n);
  std::vector<int> indices(n);
  FillWithRandomVolumes(&volumes[0], &data[0], n, &bvh.bbox, &indices[0]);
  GeoHBInsert(&bvh, n, &volumes[0], &data[0]);
  struct CountIntersectingVolumesCtx ctx;
  ctx.intersecting_volumes = 0;
  struct GeoBoundingBox v = {{0.25, 1.35, 0.8}, {0, 0, 0}};
  double my_eps = 1.0e-6;
  v.max = {v.min.x + my_eps, v.min.y + my_eps, v.min.z + my_eps};
  EXPECT_NO_THROW(
      GeoHBVisitIntersectingVolumes(
          &bvh, &v, CountTraversedNodes, &ctx));
}

TEST_F(HashedBvh, VisitMediumVolumeIntersection) {
  int n = 10;
  std::vector<struct GeoBoundingBox> volumes(n,
      {{0.21, 1.4, 0.4}, {0.3, 1.8, 0.7}});
  std::vector<void*> data(n);
  std::vector<int> indices(n);
  FillWithRandomVolumes(&volumes[0], &data[0], n, &bvh.bbox, &indices[0]);
  GeoHBInsert(&bvh, n, &volumes[0], &data[0]);
  struct CountIntersectingVolumesCtx ctx;
  ctx.intersecting_volumes = 0;
  struct GeoBoundingBox v = {{0.25, 1.35, 0.8}, {0, 0, 0}};
  double my_eps = 1.0e-2;
  v.max = {v.min.x + my_eps, v.min.y + my_eps, v.min.z + my_eps};
  EXPECT_NO_THROW(
      GeoHBVisitIntersectingVolumes(
          &bvh, &v, CountTraversedNodes, &ctx));
}
