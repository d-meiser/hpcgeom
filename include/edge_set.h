#ifndef EDGE_SET_H
#define EDGE_SET_H

#include <geo_export.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoEdgeSet {
	struct GeoVertexSet *vs;
};

GEO_EXPORT void GeoESInitialize(struct GeoEdgeSet *es,
	struct GeoVertexSet *vs);
GEO_EXPORT void GeoESDestroy(struct GeoEdgeSet *es);

#ifdef __cplusplus
}
#endif

#endif
