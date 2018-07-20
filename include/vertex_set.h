#ifndef VERTEX_SET_H
#define VERTEX_SET_H

#include <basic_types.h>
#include <hash_table.h>
#include <hashed_octree.h>
#include <vertex_array.h>
#include <geo_export.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoVertexSet
{
	struct GeoHashedOctree octree;
	struct GeoVertexArray short_list;
	struct GeoVertexData *vertex_data;
	uint32_t size;
	uint32_t capacity;
	GeoId next_id;
	double epsilon;
	struct GeoHashTable id_map;
};

GEO_EXPORT void GeoVSInitialize(struct GeoVertexSet* vs,
	struct GeoBoundingBox bbox, double epsilon);
GEO_EXPORT void GeoVSDestroy(struct GeoVertexSet* vs);
GEO_EXPORT void GeoVSInsert(struct GeoVertexSet *vs,
	struct GeoPoint p, GeoId *id);
GEO_EXPORT struct GeoVertex GeoVSGetVertex(struct GeoVertexSet *vs,
	GeoId id, int *have_vertex);
GEO_EXPORT void GeoVSOptimize(struct GeoVertexSet *vs);

#ifdef __cplusplus
}
#endif

#endif

