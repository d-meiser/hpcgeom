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
  int count;
};

int CountTraversedNodes(struct GeoBoundingBox *volumes, void **data, int i,
                        void *ctx) {
  (void)volumes;
  (void)data;
  (void)i;
  struct CountIntersectingVolumesCtx* count_ctx =
      static_cast<CountIntersectingVolumesCtx*>(ctx);
  ++count_ctx->count;
  return 1;
}

} // extern "C"

static void scale_bbox(struct GeoBoundingBox *bbox, double scale_factor) {
  double centers[3] = {
      0.5 * (bbox->min.x + bbox->max.x),
      0.5 * (bbox->min.y + bbox->max.y),
      0.5 * (bbox->min.z + bbox->max.z)};
  double lengths[3] = {
      bbox->max.x - bbox->min.x,
      bbox->max.y - bbox->min.y,
      bbox->max.z - bbox->min.z};
  ASSERT_GE(lengths[0], 0);
  ASSERT_GE(lengths[1], 0);
  ASSERT_GE(lengths[2], 0);
  bbox->min.x = centers[0] - 0.5 * scale_factor * lengths[0];
  bbox->min.y = centers[1] - 0.5 * scale_factor * lengths[1];
  bbox->min.z = centers[2] - 0.5 * scale_factor * lengths[2];
  bbox->max.x = centers[0] + 0.5 * scale_factor * lengths[0];
  bbox->max.y = centers[1] + 0.5 * scale_factor * lengths[1];
  bbox->max.z = centers[2] + 0.5 * scale_factor * lengths[2];
}

TEST_F(HashedBvh, VisitAnIntersectingVolume) {
  struct GeoBoundingBox volume = bvh.bbox;
  scale_bbox(&volume, 1.0e-6);
  std::vector<void*> data(1, nullptr);
  GeoHBInsert(&bvh, 1, &volume, &data[0]);
  struct CountIntersectingVolumesCtx ctx;
  ctx.count = 0;
  GeoHBVisitIntersectingVolumes(
      &bvh, &volume, CountTraversedNodes, &ctx);
  EXPECT_EQ(1, ctx.count);
}

TEST_F(HashedBvh, VisitThreeIntersectingVolume) {
  int n = 3;
  struct GeoBoundingBox volume = bvh.bbox;
  scale_bbox(&volume, 1.0e-6);
  std::vector<struct GeoBoundingBox> volumes(n, volume);
  std::vector<void*> data(n, nullptr);
  GeoHBInsert(&bvh, n, &volumes[0], &data[0]);
  struct CountIntersectingVolumesCtx ctx;
  ctx.count = 0;
  GeoHBVisitIntersectingVolumes(
      &bvh, &volume, CountTraversedNodes, &ctx);
  EXPECT_EQ(3, ctx.count);
}

TEST_F(HashedBvh, CanInsertVolumesMultipleTimes) {
  int n = 3;
  struct GeoBoundingBox volume = bvh.bbox;
  scale_bbox(&volume, 1.0e-6);
  std::vector<struct GeoBoundingBox> volumes(n, volume);
  std::vector<void*> data(n, nullptr);
  GeoHBInsert(&bvh, n, &volumes[0], &data[0]);
  GeoHBInsert(&bvh, n, &volumes[0], &data[0]);
  struct CountIntersectingVolumesCtx ctx;
  ctx.count = 0;
  GeoHBVisitIntersectingVolumes(
      &bvh, &volume, CountTraversedNodes, &ctx);
  EXPECT_EQ(6, ctx.count);
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
  ctx.count = 0;
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
  ctx.count = 0;
  struct GeoBoundingBox v = {{0.25, 1.35, 0.8}, {0, 0, 0}};
  double my_eps = 1.0e-2;
  v.max = {v.min.x + my_eps, v.min.y + my_eps, v.min.z + my_eps};
  EXPECT_NO_THROW(
      GeoHBVisitIntersectingVolumes(
          &bvh, &v, CountTraversedNodes, &ctx));
}

TEST_F(HashedBvh, VisitLargeVolumeIntersection) {
  int n = 100;
  std::vector<struct GeoBoundingBox> volumes(n,
      {{0.21, 1.4, 0.4}, {0.3, 1.8, 0.7}});
  std::vector<void*> data(n);
  std::vector<int> indices(n);
  FillWithRandomVolumes(&volumes[0], &data[0], n, &bvh.bbox, &indices[0]);
  GeoHBInsert(&bvh, n, &volumes[0], &data[0]);
  struct CountIntersectingVolumesCtx ctx;
  ctx.count = 0;
  struct GeoBoundingBox v = {{0.25, 1.35, 0.8}, {0, 0, 0}};
  double my_eps = 1.0e-1;
  v.max = {v.min.x + my_eps, v.min.y + my_eps, v.min.z + my_eps};
  EXPECT_NO_THROW(
      GeoHBVisitIntersectingVolumes(
          &bvh, &v, CountTraversedNodes, &ctx));
}
