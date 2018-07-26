#include <gtest/gtest.h>
#include <edge_array.h>

TEST(EdgeArray, Initialize) {
  struct GeoEdgeArray ea;
  GeoEAInitialize(&ea, 0);
  GeoEADestroy(&ea);
}

TEST(EdgeArray, CanResize) {
  struct GeoEdgeArray ea;
  GeoEAInitialize(&ea, 0);
  GeoEAResize(&ea, 10);
  ea.edges[0] = {{0, 0}};
  GeoEADestroy(&ea);
}


