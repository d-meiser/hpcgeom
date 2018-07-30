#ifndef EDGE_SET_H
#define EDGE_SET_H

#include <geo_export.h>
#include <basic_types.h>
#include <hash_table.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoVertexSet;

struct GeoEdgeSet {
	struct GeoEdge *edges;
	int size;
	int capacity;
	GeoEdgeId next_id;
	struct GeoHashTable id_map;
};

GEO_EXPORT void GeoESInitialize(struct GeoEdgeSet *es);
GEO_EXPORT void GeoESDestroy(struct GeoEdgeSet *es);
GEO_EXPORT GeoEdgeId GeoESInsert(struct GeoEdgeSet *es, struct GeoEdge edge);

#ifdef __cplusplus
}
#endif

#endif
