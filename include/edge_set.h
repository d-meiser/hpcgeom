#ifndef EDGE_SET_H
#define EDGE_SET_H

#include <geo_export.h>
#include <basic_types.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoVertexSet;

struct GeoEdgeSet {
	struct GeoEdge *large_list;
	int size;
	int capacity;
	struct GeoEdge *short_list;
};

GEO_EXPORT void GeoESInitialize(struct GeoEdgeSet *es);
GEO_EXPORT void GeoESDestroy(struct GeoEdgeSet *es);
GEO_EXPORT GeoEdgeId GeoESInsert(struct GeoEdgeSet *es, struct GeoEdge edge);
GEO_EXPORT struct GeoEdge *GeoESGetEdge(struct GeoEdgeSet *es, GeoEdgeId id);

#ifdef __cplusplus
}
#endif

#endif
