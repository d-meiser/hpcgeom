#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <geo_export.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


struct GeoPoint {
	double x;
	double y;
	double z;
};

struct GeoBoundingBox {
	struct GeoPoint min;
	struct GeoPoint max;
};

struct GeoEdge;
struct GeoEdgeList {
	struct GeoEdgeList *next;
	struct GeoEdge *edge;
};

typedef uint32_t GeoId;
struct GeoVertexData {
	GeoId id;
	struct GeoEdgeList *edge_list;
};

GEO_EXPORT void GeoVDInitialize(struct GeoVertexData *vd);
GEO_EXPORT void GeoVDDestroy(struct GeoVertexData *vd);

struct GeoVertex {
	struct GeoPoint p;
	struct GeoVertexData *ptr;
};

#ifdef __cplusplus
}
#endif

#endif

