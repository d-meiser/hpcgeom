#ifndef VERTEX_SET_H
#define VERTEX_SET_H

#include <basic_types.h>
#include <hashed_octree.h>
#include <vertex_array.h>
#include <geo_export.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t GeoId;
struct GeoIdTable;

struct GeoVertexSet
{
	struct GeoHashedOctree large;
	struct GeoHashedOctree small;
	struct GeoIdTable *id_table;
	double epsilon;
};

GEO_EXPORT void GeoVSInitialize(struct GeoVertexSet* vs,
	struct GeoBoundingBox bbox, double epsilon);
GEO_EXPORT void GeoVSDestroy(struct GeoVertexSet* vs);

/** Insert if the vertex isn't in the set yet.
 *
 * If the vertex is alreay in the set return the existing vertex pointer. If
 * the vertex is inserted the return pointer is equal to v->ptr. In code:
 *
 * 	struct GeoVertex v = {x, y, z, p};
 * 	void* q = GeoVSInsert(&v, &id);
 * 	if (q == p) {
 * 		// Vertex was inserted into the set.
 * 	} else {
 * 		// Vertex was already in the set. Its vertex data is given
 *		// by q.
 * 	}
 * */
GEO_EXPORT void *GeoVSInsert(struct GeoVertex v, GeoId *id);
GEO_EXPORT struct GeoVertex GeoVSGetVertexLocation(GeoId id);

#ifdef __cplusplus
}
#endif

#endif

