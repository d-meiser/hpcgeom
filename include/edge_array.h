#ifndef EDGE_ARRAY_H
#define EDGE_ARRAY_H

#include <basic_types.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoEdgeArray {
	struct GeoEdge *edges;
	int size;
	int capacity;
	struct GeoVertexSet *vertices;
};

GEO_EXPORT void GeoEAInitialize(struct GeoEdgeArray* ea,
	struct GeoVertexSet *vertices);
GEO_EXPORT void GeoEADestroy(struct GeoEdgeArray* ea);
GEO_EXPORT void GeoEAResize(struct GeoEdgeArray* ea, int size);

#ifdef __cplusplus
}
#endif

#endif

