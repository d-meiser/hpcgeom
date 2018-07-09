#include <vertex_array.h>
#include <gtest/gtest.h>

using namespace hpcgeo;


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
