#include <vertex_set.h>
#include <gtest/gtest.h>


namespace {


struct VertexSet : public testing::Test {
  struct GeoVertexSet vertex_set;
  const struct GeoBoundingBox bbox = {{1.2, 2.3, -1.3}, {3.0, 5.0, 10.0}};
  const double epsilon = 1.0e-6;

  void SetUp() override {
    GeoVSInitialize(&vertex_set, bbox, epsilon);
  }

  void TearDown() override {
    GeoVSDestroy(&vertex_set);
  }
};

TEST_F(VertexSet, Initialize) {
  EXPECT_TRUE(true);
}

TEST_F(VertexSet, CanInsertAVertex) {
  GeoId id;
  struct GeoVertexData *q =
      GeoVSInsert(&vertex_set, {3.0, 3.0, 3.0}, &id);
  EXPECT_NE((struct GeoVertexData*)0, q);
}

TEST_F(VertexSet, WhenInsertingVertexASecondTimeWeGetTheSameId) {
  GeoId id1, id2;
  struct GeoPoint pt = {3.0, 3.0, 3.0};
  GeoVSInsert(&vertex_set, pt, &id1);
  GeoVSInsert(&vertex_set, pt, &id2);
  EXPECT_EQ(id1, id2);
}

TEST_F(VertexSet, WhenInsertingDistinctItemsWeGetDifferentIds) {
  GeoId id1, id2;
  GeoVSInsert(&vertex_set, {3.0, 3.0, 3.0}, &id1);
  GeoVSInsert(&vertex_set,
      {3.0 + 3 * epsilon, 3.0 + 3 * epsilon, 3.0 + 3 * epsilon}, &id2);
  EXPECT_NE(id1, id2);
}

}
