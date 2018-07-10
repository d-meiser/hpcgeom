#include <vertex_array.h>
#include <gtest/gtest.h>
#include <random>
#include <algorithm>


using namespace hpcgeo;


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

void fill_random(VertexArray* va) {
  fill_with_random_doubles(va->x(), va->x() + va->size());
  fill_with_random_doubles(va->y(), va->y() + va->size());
  fill_with_random_doubles(va->z(), va->z() + va->size());
  fill_with_random_uint64_t(va->ptrs(), va->ptrs() + va->size());
}


TEST(VertexArray, InitiallyHasSizeZero) {
  EXPECT_EQ(0, VertexArray().size());
}

TEST(VertexArray, PointersAreInitiallyAligned) {
  VertexArray va;
  EXPECT_EQ(0, reinterpret_cast<uint64_t>(va.x()) % VertexArray::ALIGNMENT);
  EXPECT_EQ(0, reinterpret_cast<uint64_t>(va.y()) % VertexArray::ALIGNMENT);
  EXPECT_EQ(0, reinterpret_cast<uint64_t>(va.z()) % VertexArray::ALIGNMENT);
  EXPECT_EQ(0, reinterpret_cast<uint64_t>(va.ptrs()) % VertexArray::ALIGNMENT);
}

TEST(VertexArray, PointersAreAlignedAfterResize) {
  VertexArray va;
  va.resize(200);
  EXPECT_EQ(0, reinterpret_cast<uint64_t>(va.x()) % VertexArray::ALIGNMENT);
  EXPECT_EQ(0, reinterpret_cast<uint64_t>(va.y()) % VertexArray::ALIGNMENT);
  EXPECT_EQ(0, reinterpret_cast<uint64_t>(va.z()) % VertexArray::ALIGNMENT);
  EXPECT_EQ(0, reinterpret_cast<uint64_t>(va.ptrs()) % VertexArray::ALIGNMENT);
}

TEST(VertexArray, CanCopy) {
  VertexArray va;
  va.resize(10);
  fill_random(&va);
  VertexArray va2(va);
  EXPECT_NEAR(va.x()[0], va2.x()[0], eps);
  EXPECT_NEAR(va.x()[1], va2.x()[1], eps);
  EXPECT_NEAR(va.y()[0], va2.y()[0], eps);
  EXPECT_NEAR(va.y()[1], va2.y()[1], eps);
  EXPECT_NEAR(va.z()[0], va2.z()[0], eps);
  EXPECT_NEAR(va.z()[1], va2.z()[1], eps);
  EXPECT_EQ(va.ptrs()[0], va2.ptrs()[0]);
  EXPECT_EQ(va.ptrs()[1], va2.ptrs()[1]);

  double orig_x = va.x()[0];
  va2.x()[0] += 1.0;
  EXPECT_NE(orig_x, va2.x()[0]);
  EXPECT_EQ(orig_x, va.x()[0]);
}

TEST(VertexArray, CanMove) {
  VertexArray va;
  va.resize(10);
  fill_random(&va);
  VertexArray va2(std::move(va));
  EXPECT_EQ(10, va2.size());
}

}
