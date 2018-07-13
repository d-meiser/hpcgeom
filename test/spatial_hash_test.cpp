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

TEST(GeoNodeKey, ZeroIsNotValidNode) {
  EXPECT_FALSE(GeoNodeValidKey(0u));
}

TEST(GeoNodeKey, RootNodeIsValid) {
  EXPECT_TRUE(GeoNodeValidKey(GeoNodeRoot()));
}

TEST(GeoNodeKey, RootNodeIsLevelZero) {
  EXPECT_EQ(0, GeoNodeLevel(GeoNodeRoot()));
}

TEST(GeoNodeKey, ChildrenOfRootAreAtLevelOne) {
  GeoNodeKey child_keys[8];
  GeoNodeComputeChildKeys(GeoNodeRoot(), child_keys);
  EXPECT_EQ(1, GeoNodeLevel(child_keys[0]));
}

TEST(GeoNodeKey, ParentIsAtLowerLevel) {
  GeoNodeKey key = 64u;
  ASSERT_TRUE(GeoNodeValidKey(key));
  EXPECT_LT(GeoNodeLevel(GeoNodeParent(key)), GeoNodeLevel(key));
}

TEST(GeoNodeKey, BeginOfRootIsZero) {
  EXPECT_EQ(0u, GeoNodeBegin(GeoNodeRoot()));
}

TEST(GeoNodeKey, EndOfRootIsTwoToTheThirty) {
  EXPECT_EQ(1u << (3 * 10), GeoNodeEnd(GeoNodeRoot()));
}

TEST(GeoNodeKey, BeginSpotChecks) {
  EXPECT_EQ(1u << (3 * 9), GeoNodeBegin(9u));
  EXPECT_EQ(2u << (3 * 9), GeoNodeBegin(10u));
  EXPECT_EQ(3u << (3 * 9), GeoNodeBegin(11u));
  EXPECT_EQ(1u << (3 * 8), GeoNodeBegin(65u));
  EXPECT_EQ(10u << (3 * 8), GeoNodeBegin(74u));
}

}

