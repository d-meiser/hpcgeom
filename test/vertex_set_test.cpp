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
  int id;
  void *p = (void*)0x1;
  void *q = GeoVSInsert({{3.0, 3.0, 3.0}, p}, &id);
  EXPECT_NE(p, q);
}

}
