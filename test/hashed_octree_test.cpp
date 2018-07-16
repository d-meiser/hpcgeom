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
  GeoHOInsert(&octree, &vertex_array);
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
  GeoHOInsert(&octree, &vertex_array);
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

int CountVisits(struct GeoVertexArray *va, int i, void* ctx) {
  struct CountVisitsCtx *count_visits_ctx = (struct CountVisitsCtx*)ctx;
  if (va->ptrs[i] != count_visits_ctx->self_ptr) {
    ++count_visits_ctx->visits;
  }
  return 1;
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

  GeoHOInsert(&octree, &vertex_array);

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

  GeoHOInsert(&octree, &vertex_array);

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

  GeoHOInsert(&octree, &vertex_array);

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

  GeoHOInsert(&octree, &vertex_array);

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

  GeoHOInsert(&octree, &vertex_array);

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
  GeoHOInsert(&octree, &vertex_array);
  // Have to set the pointes to something different so CountVisits
  // doesn't identify the vertices as the same vertex.
  for (int i = 0; i < num_vertices; ++i) {
    vertex_array.ptrs[i] = &indices[num_vertices + i];
  }
  GeoHOInsert(&octree, &vertex_array);

  struct CountVisitsCtx ctx = {&indices[0], 0};
  struct GeoPoint p = {
    vertex_array.x[0], vertex_array.y[0], vertex_array.z[0]};
  GeoHOVisitNearVertices(&octree, &p, eps,
                         CountVisits, &ctx);
  EXPECT_GT(ctx.visits, 0);
}

void TrivialDtor(void *, void *) {}

int count_close_pairs(const struct GeoVertexArray *va, double my_eps) {
  int n = 0;
  for (int i = 0; i < va->size; ++i) {
    for (int j = 0; j < i; ++j) {
      double dx = fabs(va->x[i] - va->x[j]);
      double dy = fabs(va->y[i] - va->y[j]);
      double dz = fabs(va->z[i] - va->z[j]);
      if (dx <= my_eps && dy <= my_eps && dz <= my_eps) {
        printf("Found duplicate\n");
        printf("i == %d, j == %d\n", i, j);
        printf("dx = %lf, dy = %lf, dz = %lf\n", dx, dy, dz);
        printf("  vertex_array.x[0] = %.14le;\n", va->x[i]);
        printf("  vertex_array.y[0] = %.14le;\n", va->y[i]);
        printf("  vertex_array.z[0] = %.14le;\n", va->z[i]);
        printf("  vertex_array.x[1] = %.14le;\n", va->x[j]);
        printf("  vertex_array.y[1] = %.14le;\n", va->y[j]);
        printf("  vertex_array.z[1] = %.14le;\n", va->z[j]);
        ++n;
      }
    }
  }
  return n;
}

TEST_F(HashedOctree, NoDuplicatesAfterDeduplicateSimple1) {
  int num_vertices = 2;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  vertex_array.x[0] = 0.1;
  vertex_array.y[0] = 0.2;
  vertex_array.z[0] = 0.3;
  vertex_array.x[1] = vertex_array.x[0];
  vertex_array.y[1] = vertex_array.y[0];
  vertex_array.z[1] = vertex_array.z[0];
  GeoHOInsert(&octree, &vertex_array);
  double my_eps = 1.0e-3;
  GeoHODeleteDuplicates(&octree, my_eps, TrivialDtor, 0);
  EXPECT_EQ(1, octree.vertices.size);
  EXPECT_EQ(0, count_close_pairs(&octree.vertices, my_eps));
}

TEST_F(HashedOctree, NoDuplicatesAfterDeduplicateSimple2) {
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
  GeoHOInsert(&octree, &vertex_array);
  double my_eps = 1.0e-3;
  GeoHODeleteDuplicates(&octree, my_eps, TrivialDtor, 0);
  EXPECT_EQ(2, octree.vertices.size);
  EXPECT_EQ(0, count_close_pairs(&octree.vertices, my_eps));
}

TEST_F(HashedOctree, NoDuplicatesAfterDeduplicateSimple3) {
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
  vertex_array.x[2] = vertex_array.x[0] + 0.1 * eps;
  vertex_array.y[2] = vertex_array.y[0];
  vertex_array.z[2] = vertex_array.z[0];
  GeoHOInsert(&octree, &vertex_array);
  double my_eps = 1.0e-3;
  GeoHODeleteDuplicates(&octree, my_eps, TrivialDtor, 0);
  EXPECT_EQ(1, octree.vertices.size);
  EXPECT_EQ(0, count_close_pairs(&octree.vertices, my_eps));
}

TEST_F(HashedOctree, NoDuplicatesAfterDeduplicateSimple4) {
  int num_vertices = 2;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  vertex_array.x[0] = 3.63289380072066e-01;
  vertex_array.y[0] = 4.13765990715816e-01;
  vertex_array.z[0] = 8.57429681233387e-01;
  vertex_array.x[1] = 3.62965736776768e-01;
  vertex_array.y[1] = 4.20491496714617e-01;
  vertex_array.z[1] = 8.47800529558627e-01;
  GeoHOInsert(&octree, &vertex_array);
  double my_eps = 1.0e-2;
  GeoHODeleteDuplicates(&octree, my_eps, TrivialDtor, 0);
  EXPECT_EQ(1, octree.vertices.size);
  EXPECT_EQ(0, count_close_pairs(&octree.vertices, my_eps));
}

TEST_F(HashedOctree, NoDuplicatesAfterDeduplicate) {
  int num_vertices = 500;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  GeoHOInsert(&octree, &vertex_array);
  double my_eps = 1.0e-2;
  GeoHODeleteDuplicates(&octree, my_eps, TrivialDtor, 0);
  EXPECT_EQ(0, count_close_pairs(&octree.vertices, my_eps));
}

TEST_F(HashedOctree, CanDealWithManyDuplicates) {
  int num_vertices = 200;
  GeoVAResize(&vertex_array, num_vertices);
  indices.resize(num_vertices);
  FillWithRandomItems(&vertex_array, &octree.bbox, num_vertices, &indices[0]);
  for (int i = 0; i < num_vertices; ++i) {
    vertex_array.x[i] = 0.2;
    vertex_array.y[i] = 0.3;
    vertex_array.z[i] = 0.4;
  }
  GeoHOInsert(&octree, &vertex_array);
  double my_eps = 1.0e-2;
  GeoHODeleteDuplicates(&octree, my_eps, TrivialDtor, 0);
  EXPECT_EQ(0, count_close_pairs(&octree.vertices, my_eps));
}

}
