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
    GeoESInitialize(&edge_set);
  }

  void TearDown() override {
    GeoESDestroy(&edge_set);
    GeoVSDestroy(&vertex_set);
  }
};

struct GeoBoundingBox EdgeSet::bbox = {{-0.4, 0.3, 1.7}, {1.1, 0.4, 3.0}};
const double EdgeSet::my_eps = 1.0e-4;

TEST_F(EdgeSet, CanInsertAnEdge) {
  EXPECT_NO_THROW(GeoESInsert(&edge_set, {{0, 1}}));
}

TEST_F(EdgeSet, InsertingSameEdgeTwiceGivesSameEdge) {
  GeoEdgeId my_edge1 = GeoESInsert(&edge_set, {{0, 1}});
  GeoEdgeId my_edge2 = GeoESInsert(&edge_set, {{0, 1}});
  EXPECT_EQ(my_edge1, my_edge2);
}

TEST_F(EdgeSet, InsertingEdgeWithVerticesReversedGivesSameEdge) {
  GeoEdgeId my_edge1 = GeoESInsert(&edge_set, {{0, 1}});
  GeoEdgeId my_edge2 = GeoESInsert(&edge_set, {{1, 0}});
  EXPECT_EQ(my_edge1, my_edge2);
}

TEST_F(EdgeSet, InsertingDifferentEdgesGivesDistinctIds) {
  GeoEdgeId my_edge1 = GeoESInsert(&edge_set, {{0, 1}});
  GeoEdgeId my_edge2 = GeoESInsert(&edge_set, {{0, 2}});
  EXPECT_NE(my_edge1, my_edge2);
}

TEST_F(EdgeSet, CanInsertManyEdges) {
  const int num_edges = 100;
  std::vector<GeoEdgeId> edges(num_edges);
  for (int i = 0; i < num_edges; ++i) {
    edges[i] = GeoESInsert(&edge_set, {{(GeoVertexId)i, (GeoVertexId)(i + 1)}});
  }
  for (int i = 0; i < num_edges; ++i) {
    for (int j = 0; j < i; ++j) {
      EXPECT_NE(edges[i], edges[j]);
    }
  }
  for (int i = 0; i < num_edges; ++i) {
    struct GeoEdge *edge = GeoESGetEdge(&edge_set, edges[i]);
    EXPECT_NE(static_cast<struct GeoEdge*>(0x0), edge) << i;
  }
}


