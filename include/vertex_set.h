#ifndef VERTEX_SET_H
#define VERTEX_SET_H

#include <basic_types.h>
#include <hashed_octree.h>
#include <vertex_array.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoVertexSet
{
	struct GeoHashedOctree large;
	struct GeoHashedOctree small;
	uint32_t *locations;
	double epsilon;
};

void GeoVSInitialize(struct GeoVertexSet* vs, struct GeoBoundingBox bbox,
	double epsilon);
void GeoVSDestroy(struct GeoVertexSet* vs);

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
void* GeoVSInsert(struct GeoVertex* v, int* id);
struct GeoVertex GeoVSGetVertexLocation(int id);

#ifdef __cplusplus
}
#endif

#endif

