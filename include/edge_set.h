#ifndef EDGE_SET_H
#define EDGE_SET_H

#include <geo_export.h>
#include <basic_types.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoVertexSet;
struct GeoHashedBvh;

struct GeoEdgeSet {
	struct GeoVertexSet *vs;
	struct GeoHashedBvh *bvh;
};

GEO_EXPORT void GeoESInitialize(struct GeoEdgeSet *es,
	struct GeoVertexSet *vs);
GEO_EXPORT void GeoESDestroy(struct GeoEdgeSet *es);
GEO_EXPORT void GeoESInsert(struct GeoEdgeSet *es,
	struct GeoPoint p1, struct GeoPoint p2, GeoEdgeId *id);

#ifdef __cplusplus
}
#endif

#endif
