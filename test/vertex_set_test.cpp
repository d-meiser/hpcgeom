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
  GeoVertexId id;
  GeoVSInsert(&vertex_set, {3.0, 3.0, 3.0}, &id);
}

TEST_F(VertexSet, WhenInsertingVertexASecondTimeWeGetTheSameId) {
  GeoVertexId id1, id2;
  struct GeoPoint pt = {3.0, 3.0, 3.0};
  GeoVSInsert(&vertex_set, pt, &id1);
  GeoVSInsert(&vertex_set, pt, &id2);
  EXPECT_EQ(id1, id2);
}

TEST_F(VertexSet, WhenInsertingDistinctItemsWeGetDifferentIds) {
  GeoVertexId id1, id2;
  GeoVSInsert(&vertex_set, {3.0, 3.0, 3.0}, &id1);
  GeoVSInsert(&vertex_set,
      {3.0 + 3 * epsilon, 3.0 + 3 * epsilon, 3.0 + 3 * epsilon}, &id2);
  EXPECT_NE(id1, id2);
}

TEST_F(VertexSet, EmptySetDoesntHaveVertex) {
  int have_vertex;
  GeoVSGetVertex(&vertex_set, 10, &have_vertex);
  EXPECT_EQ(0, have_vertex);
}

TEST_F(VertexSet, CanRetrieveVertex) {
  struct GeoPoint p = {1.0, 2.0, 3.0};
  GeoVertexId id;
  GeoVSInsert(&vertex_set, p, &id);
  int have_vertex;
  struct GeoVertex v = GeoVSGetVertex(&vertex_set, id, &have_vertex);
  EXPECT_NE(0, have_vertex);
  EXPECT_NEAR(p.x, v.p.x, 1.0e-10);
  EXPECT_NEAR(p.y, v.p.y, 1.0e-10);
  EXPECT_NEAR(p.z, v.p.z, 1.0e-10);
}

TEST_F(VertexSet, CanOptimize) {
  GeoVertexId id1, id2;
  GeoVSInsert(&vertex_set, {3.0, 3.0, 3.0}, &id1);
  GeoVSInsert(&vertex_set,
      {3.0 + 3 * epsilon, 3.0 + 3 * epsilon, 3.0 + 3 * epsilon}, &id2);
  GeoVSOptimize(&vertex_set);
  EXPECT_NE(id1, id2);
}

}
