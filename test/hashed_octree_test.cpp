#include <gtest/gtest.h>
#include <hashed_octree.h>

namespace {

struct HashedOctree : public ::testing::Test {
  struct GeoHashedOctree octree;
  struct GeoVertexArray vertex_array;

  void SetUp() override {
    GeoHOInitialize(&octree, {{0, 0, 0}, {1, 1, 1}});
    GeoVAInitialize(&vertex_array);
  }
  void TearDown() override {
    GeoVADestroy(&vertex_array);
    GeoHODestroy(&octree);
  }
};

TEST_F(HashedOctree, Constructor) {
}

TEST_F(HashedOctree, CanInsertItems) {
  int num_vertices = 2;
  GeoVAResize(&vertex_array, num_vertices);
  vertex_array.x[0] = 0.2;
  vertex_array.y[0] = 0.2;
  vertex_array.z[0] = 0.2;
  vertex_array.ptrs[0] = 0;
  vertex_array.y[1] = 0.3;
  vertex_array.z[1] = 0.3;
  vertex_array.x[1] = 0.3;
  vertex_array.ptrs[1] = 0;
  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);
  EXPECT_EQ(2, octree.vertices.size);
}

}
