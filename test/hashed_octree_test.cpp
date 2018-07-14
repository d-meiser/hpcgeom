#include <gtest/gtest.h>
#include <hashed_octree.h>

namespace {

const double eps = 1.0e-15;


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
  vertex_array.ptrs[0] = (void*)0x10ull;
  vertex_array.y[1] = 0.3;
  vertex_array.z[1] = 0.3;
  vertex_array.x[1] = 0.3;
  vertex_array.ptrs[1] = (void*)0xF0ull;
  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);
  EXPECT_EQ(2, octree.vertices.size);
  EXPECT_NEAR(0.2, octree.vertices.x[0], eps);
  EXPECT_NEAR(0.2, octree.vertices.y[0], eps);
  EXPECT_NEAR(0.2, octree.vertices.z[0], eps);
  EXPECT_EQ(0x10ull, (uint64_t)octree.vertices.ptrs[0]);
  EXPECT_NEAR(0.3, octree.vertices.x[1], eps);
  EXPECT_NEAR(0.3, octree.vertices.y[1], eps);
  EXPECT_NEAR(0.3, octree.vertices.z[1], eps);
  EXPECT_EQ(0xF0ull, (uint64_t)octree.vertices.ptrs[1]);
}

}
