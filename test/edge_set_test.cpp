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

TEST_F(EdgeSet, CanInsertAnEdge) {
  GeoEdgeId my_edge;
  GeoESInsert(&edge_set, {0.1, 0.31, 1.8}, {0.2, 0.3, 2.1}, &my_edge);
}

TEST_F(EdgeSet, InsertingSameEdgeTwiceGivesSameEdge) {
  GeoEdgeId my_edge1, my_edge2;
  GeoPoint p1 = {0.1, 0.31, 1.8};
  GeoPoint p2 = {0.2, 0.3, 2.1};
  GeoESInsert(&edge_set, p1, p2, &my_edge1);
  GeoESInsert(&edge_set, p1, p2, &my_edge2);
  EXPECT_EQ(my_edge1, my_edge2);
}

TEST_F(EdgeSet, InsertingEdgeWithVerticesReversedGivesSameEdge) {
  GeoEdgeId my_edge1, my_edge2;
  GeoPoint p1 = {0.1, 0.31, 1.8};
  GeoPoint p2 = {0.2, 0.3, 2.1};
  GeoESInsert(&edge_set, p1, p2, &my_edge1);
  GeoESInsert(&edge_set, p2, p1, &my_edge2);
  EXPECT_EQ(my_edge1, my_edge2);
}

TEST_F(EdgeSet, DISABLED_WIP_InsertingDifferentEdgesGivesDistinctIds) {
  GeoEdgeId my_edge1, my_edge2;
  GeoESInsert(&edge_set, {0.1, 0.31, 1.8}, {0.2, 0.3, 2.1}, &my_edge1);
  GeoESInsert(&edge_set, {0.1, 0.31, 1.8}, {0.2 + 2.0 * my_eps, 0.3, 2.1},
      &my_edge2);
  EXPECT_NE(my_edge1, my_edge2);
}


