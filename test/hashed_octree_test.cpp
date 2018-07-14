#include <gtest/gtest.h>
#include <hashed_octree.h>
#include <test_utilities.h>
#include <random>


namespace {

const double eps = 1.0e-15;
std::random_device rd;
std::mt19937 gen(rd());



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

TEST_F(HashedOctree, InsertedItemsAreSorted) {
  int num_vertices = 200;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);
  for (int i = 0; i < num_vertices - 1; ++i) {
    EXPECT_LE(octree.hashes[i], octree.hashes[i + 1]) <<
        ">>> i == " << i;
  }
}

extern "C" {

struct CountVisitsCtx {
  void* self_ptr;
  int visits;
};

void CountVisits(struct GeoVertexArray *va, int i, void* ctx) {
  struct CountVisitsCtx *count_visits_ctx = (struct CountVisitsCtx*)ctx;
  if (va->ptrs[i] != count_visits_ctx->self_ptr) {
    ++count_visits_ctx->visits;
  }
}

}

TEST_F(HashedOctree, VisitsNearbyVertexManufactured) {
  int num_vertices = 3;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  vertex_array.x[0] = 0.1;
  vertex_array.y[0] = 0.2;
  vertex_array.z[0] = 0.3;
  vertex_array.x[1] = vertex_array.x[0] + 0.1 * eps;
  vertex_array.y[1] = vertex_array.y[0];
  vertex_array.z[1] = vertex_array.z[0];
  vertex_array.x[2] = 0.01;
  vertex_array.y[2] = 0.01;
  vertex_array.z[2] = 0.01;

  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);

  struct CountVisitsCtx ctx = {octree.vertices.ptrs[0], 0};
  struct GeoPoint p = {
    vertex_array.x[0], vertex_array.y[0], vertex_array.z[0]};
  GeoHOVisitNearVertices(&octree, &p, eps,
                         CountVisits, &ctx);
  EXPECT_GT(ctx.visits, 0);
}

TEST_F(HashedOctree, VisitsNearbyVertex) {
  int num_vertices = 100;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  vertex_array.x[0] = 0.1;
  vertex_array.y[0] = 0.2;
  vertex_array.z[0] = 0.3;
  vertex_array.x[1] = vertex_array.x[0] + 0.1 * eps;
  vertex_array.y[1] = vertex_array.y[0];
  vertex_array.z[1] = vertex_array.z[0];

  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);

  struct CountVisitsCtx ctx = {octree.vertices.ptrs[0], 0};
  struct GeoPoint p = {
    vertex_array.x[0], vertex_array.y[0], vertex_array.z[0]};
  GeoHOVisitNearVertices(&octree, &p, eps,
                         CountVisits, &ctx);
  EXPECT_GT(ctx.visits, 0);
}

TEST_F(HashedOctree, VertexInNeighbouringNodeIsVisitedX) {
  int num_vertices = 100;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  vertex_array.x[0] = 0.5 - 0.2 * eps;
  vertex_array.y[0] = 0.1;
  vertex_array.z[0] = 0.1;
  vertex_array.x[1] = 0.5 + 0.2 * eps;
  vertex_array.y[1] = 0.1;
  vertex_array.z[1] = 0.1;

  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);

  struct CountVisitsCtx ctx = {&indices[0], 0};
  struct GeoPoint p = {
    vertex_array.x[0], vertex_array.y[0], vertex_array.z[0]};
  GeoHOVisitNearVertices(&octree, &p, eps,
                         CountVisits, &ctx);
  EXPECT_GT(ctx.visits, 0);
}

TEST_F(HashedOctree, VertexInNeighbouringNodeIsVisitedY) {
  int num_vertices = 100;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  vertex_array.x[0] = 0.1;
  vertex_array.y[0] = 0.5 - 0.2 * eps;
  vertex_array.z[0] = 0.1;
  vertex_array.x[1] = 0.1;
  vertex_array.y[1] = 0.5 + 0.2 * eps;
  vertex_array.z[1] = 0.1;

  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);

  struct CountVisitsCtx ctx = {&indices[0], 0};
  struct GeoPoint p = {
    vertex_array.x[0], vertex_array.y[0], vertex_array.z[0]};
  GeoHOVisitNearVertices(&octree, &p, eps,
                         CountVisits, &ctx);
  EXPECT_GT(ctx.visits, 0);
}

TEST_F(HashedOctree, VertexInNeighbouringNodeIsVisitedZ) {
  int num_vertices = 100;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  vertex_array.x[0] = 0.1;
  vertex_array.y[0] = 0.1;
  vertex_array.z[0] = 0.5 - 0.2 * eps;
  vertex_array.x[1] = 0.1;
  vertex_array.y[1] = 0.1;
  vertex_array.z[1] = 0.5 + 0.2 * eps;

  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);

  struct CountVisitsCtx ctx = {&indices[0], 0};
  struct GeoPoint p = {
    vertex_array.x[0], vertex_array.y[0], vertex_array.z[0]};
  GeoHOVisitNearVertices(&octree, &p, eps,
                         CountVisits, &ctx);
  EXPECT_GT(ctx.visits, 0);
}

TEST_F(HashedOctree, CanInsertMultipleTimes) {
  int num_vertices = 100;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(2 * num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);
  // Have to set the pointes to something different so CountVisits
  // doesn't identify the vertices as the same vertex.
  for (int i = 0; i < num_vertices; ++i) {
    vertex_array.ptrs[i] = &indices[num_vertices + i];
  }
  GeoHOInsert(&octree, &vertex_array, 0, num_vertices);

  struct CountVisitsCtx ctx = {&indices[0], 0};
  struct GeoPoint p = {
    vertex_array.x[0], vertex_array.y[0], vertex_array.z[0]};
  GeoHOVisitNearVertices(&octree, &p, eps,
                         CountVisits, &ctx);
  EXPECT_GT(ctx.visits, 0);
}

}
