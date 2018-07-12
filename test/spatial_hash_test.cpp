#include <gtest/gtest.h>
#include <spatial_hash.h>
#include <basic_types.h>


namespace {

static const double eps = 1.0e-15;

TEST(ComputeHash, IsNullAtOrigin) {
  struct GeoBoundingBox bbox{{0, 0, 0}, {1, 1, 1}};
  struct GeoPoint p{eps, eps, eps};
  GeoSpatialHash h = GeoComputeHash(&bbox, &p);
  EXPECT_EQ(0u, h);
}

TEST(ComputeHash, IsNotNullAwayFromOrigin) {
  struct GeoBoundingBox bbox{{0, 0, 0}, {1, 1, 1}};
  struct GeoPoint p{0.5, 0.5, 0.5};
  GeoSpatialHash k = GeoComputeHash(&bbox, &p);
  EXPECT_NE(0u, k);
}

TEST(ComputeHash, SubstantiallyDifferentPointsYieldDifferentKeys) {
  struct GeoBoundingBox bbox{{0, 0, 0}, {1, 1, 1}};
  struct GeoPoint p0{0.5, 0.5, 0.5};
  GeoSpatialHash k1 = GeoComputeHash(&bbox, &p0);
  struct GeoPoint p1{0.6, 0.6, 0.6};
  GeoSpatialHash k2 = GeoComputeHash(&bbox, &p1);
  EXPECT_NE(k1, k2);
}

TEST(ComputeHash, CanComputeKeysOutsideOfBBox) {
  struct GeoBoundingBox bbox{{0, 0, 0}, {1, 1, 1}};
  struct GeoPoint p0{1.5, 1.5, 1.5};
  EXPECT_GE(GeoComputeHash(&bbox, &p0), 0u);
  struct GeoPoint p1{-1.5, -1.5, -1.5};
  EXPECT_GE(GeoComputeHash(&bbox, &p1), 0u);
  struct GeoPoint p2{-1.5, 1.5, -1.5};
  EXPECT_GE(GeoComputeHash(&bbox, &p2), 0u);
}


}

