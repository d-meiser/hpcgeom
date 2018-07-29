#include <gtest/gtest.h>
#include <edge_set.h>
#include <vertex_set.h>
#include <basic_types.h>


struct EdgeSet : public ::testing::Test {
  struct GeoVertexSet vertex_set;
  struct GeoEdgeSet edge_set;
  static struct GeoBoundingBox bbox;
  static const double my_eps;

  void SetUp() override {
    GeoVSInitialize(&vertex_set, bbox, my_eps);
    GeoESInitialize(&edge_set, &vertex_set);
  }

  void TearDown() override {
    GeoESDestroy(&edge_set);
    GeoVSDestroy(&vertex_set);
  }
};

struct GeoBoundingBox EdgeSet::bbox = {{-0.4, 0.3, 1.7}, {1.1, 0.4, 3.0}};
const double EdgeSet::my_eps = 1.0e-4;

TEST_F(EdgeSet, Include) {
  EXPECT_TRUE(true);
}


