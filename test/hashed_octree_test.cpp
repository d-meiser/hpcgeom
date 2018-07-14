#include <gtest/gtest.h>
#include <hashed_octree.h>
#include <random>


namespace {

const double eps = 1.0e-15;
std::random_device rd;
std::mt19937 gen(rd());


struct GeoBoundingBox UnitCube() {
  return {{0, 0, 0,}, {1, 1, 1}};
}

struct GeoVertexArray BuildVerticesAtRandomLocations(
    const struct GeoBoundingBox* bbox, int n, int *indices) {
  struct GeoVertexArray va;
  GeoVAInitialize(&va);
  GeoVAResize(&va, n);
  std::uniform_real_distribution<> dist_x(bbox->min.x, bbox->max.x);
  std::uniform_real_distribution<> dist_y(bbox->min.y, bbox->max.y);
  std::uniform_real_distribution<> dist_z(bbox->min.z, bbox->max.z);
  for (int i = 0; i < n; ++i) {
    va.x[i] = dist_x(gen);
    va.y[i] = dist_y(gen);
    va.z[i] = dist_z(gen);
    va.ptrs[i] = indices + i;
  }
  return va;
}

void FillTreeWithRandomItems(
    struct GeoHashedOctree *tree, int n, int *indices) {
  for (int i = 0; i < n; ++i) {
    indices[i] = i;
  }

  struct GeoVertexArray va = BuildVerticesAtRandomLocations(
      &tree->bbox, n, indices);

  GeoHOInsert(tree, &va, 0, n);
  GeoVADestroy(&va);
}


struct HashedOctree : public ::testing::Test {
  struct GeoHashedOctree octree;
  struct GeoVertexArray vertex_array;
  std::vector<int> indices;

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

extern "C" {

struct CountVisitsCtx {
  void* self_ptr;
  int visits;
};

void CountVisits(double x, double y, double z, void* ptr, void* ctx) {
  struct CountVisitsCtx *count_visits_ctx = (struct CountVisitsCtx*)ctx;
  if (ptr != count_visits_ctx->self_ptr) {
    ++count_visits_ctx->visits;
  }
}

}

TEST_F(HashedOctree, VertexInNeighbouringNodeIsVisitedX) {
  indices.resize(100);
  FillTreeWithRandomItems(&octree, 100, &indices[0]);
  octree.vertices.x[0] = 0.5 - 0.2 * eps;
  octree.vertices.y[0] = 0.1;
  octree.vertices.z[0] = 0.1;
  octree.vertices.x[1] = 0.5 + 0.2 * eps;
  octree.vertices.y[1] = 0.1;
  octree.vertices.z[1] = 0.1;

  struct CountVisitsCtx ctx = {octree.vertices.ptrs[0], 0};
  struct GeoPoint p = {
    octree.vertices.x[0], octree.vertices.y[0], octree.vertices.z[0]};
  GeoHOVisitNearVertices(&octree, &p, eps,
                         CountVisits, &ctx);
  EXPECT_GT(ctx.visits, 0);
}

}
