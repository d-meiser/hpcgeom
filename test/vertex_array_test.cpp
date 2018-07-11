#include <vertex_array.h>
#include <gtest/gtest.h>
#include <random>
#include <algorithm>



namespace {

static const double eps = 1.0e-15;

static std::random_device rd;
static std::mt19937 mte(rd());

void fill_with_random_doubles(double* start, double* end) {
  std::uniform_real_distribution<double> dist(-1.0, 1.0);
  std::generate(start, end, [&]() { return dist(mte); });
}

void fill_with_random_uint64_t(void** start, void** end) {
  std::uniform_int_distribution<uint64_t> dist(0, (uint64_t)(-1ll));
  std::generate(start, end, [&]() { return (void*)dist(mte); });
}

void fill_random(struct GeoVertexArray* va) {
  fill_with_random_doubles(va->x, va->x + va->size);
  fill_with_random_doubles(va->y, va->y + va->size);
  fill_with_random_doubles(va->z, va->z + va->size);
  fill_with_random_uint64_t(va->ptrs, va->ptrs + va->size);
}


TEST(VertexArray, InitiallyHasSizeZero) {
  struct GeoVertexArray va;
  GeoVAInitialize(&va);
  EXPECT_EQ(0, va.size);
  GeoVADestroy(&va);
}

TEST(VertexArray, PointersAreInitiallyAligned) {
  struct GeoVertexArray va;
  GeoVAInitialize(&va);
  EXPECT_EQ(0, (uint64_t)va.x % GEO_VA_ALIGNMENT);
  EXPECT_EQ(0, (uint64_t)va.y % GEO_VA_ALIGNMENT);
  EXPECT_EQ(0, (uint64_t)va.z % GEO_VA_ALIGNMENT);
  EXPECT_EQ(0, (uint64_t)va.ptrs % GEO_VA_ALIGNMENT);
  GeoVADestroy(&va);
}


TEST(VertexArray, PointersAreAlignedAfterResize) {
  struct GeoVertexArray va;
  GeoVAInitialize(&va);
  GeoVAResize(&va, 200);
  EXPECT_EQ(0, (uint64_t)va.x % GEO_VA_ALIGNMENT);
  EXPECT_EQ(0, (uint64_t)va.y % GEO_VA_ALIGNMENT);
  EXPECT_EQ(0, (uint64_t)va.z % GEO_VA_ALIGNMENT);
  EXPECT_EQ(0, (uint64_t)va.ptrs % GEO_VA_ALIGNMENT);
  GeoVADestroy(&va);
}

TEST(VertexArray, CanCopy) {
  struct GeoVertexArray va;
  GeoVAInitialize(&va);
  GeoVAResize(&va, 10);
  fill_random(&va);
  struct GeoVertexArray va2 = GeoVACopy(&va);
  EXPECT_NEAR(va.x[0], va2.x[0], eps);
  EXPECT_NEAR(va.x[1], va2.x[1], eps);
  EXPECT_NEAR(va.y[0], va2.y[0], eps);
  EXPECT_NEAR(va.y[1], va2.y[1], eps);
  EXPECT_NEAR(va.z[0], va2.z[0], eps);
  EXPECT_NEAR(va.z[1], va2.z[1], eps);
  EXPECT_EQ(va.ptrs[0], va2.ptrs[0]);
  EXPECT_EQ(va.ptrs[1], va2.ptrs[1]);

  double orig_x = va.x[0];
  va2.x[0] += 1.0;
  EXPECT_NE(orig_x, va2.x[0]);
  EXPECT_EQ(orig_x, va.x[0]);

  GeoVADestroy(&va2);
  GeoVADestroy(&va);
}

}
