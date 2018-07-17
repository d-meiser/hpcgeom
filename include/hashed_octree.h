#ifndef HASHED_OCTREE_H
#define HASHED_OCTREE_H

#include <basic_types.h>
#include <spatial_hash.h>
#include <vertex_array.h>
#include <geo_export.h>


#ifdef __cplusplus
extern "C" {
#endif


struct GeoHashedOctree
{
	struct GeoVertexArray vertices;
	GeoSpatialHash *hashes;
	struct GeoBoundingBox bbox;
};

GEO_EXPORT void GeoHOInitialize(struct GeoHashedOctree *tree,
	struct GeoBoundingBox b);
GEO_EXPORT void GeoHODestroy(struct GeoHashedOctree *tree);

GEO_EXPORT void GeoHOInsert(struct GeoHashedOctree *tree,
	const struct GeoVertexArray *va);

typedef int GeoVertexVisitor(struct GeoVertexArray *va, int i, void *ctx);
GEO_EXPORT void GeoHOVisitNearVertices(struct GeoHashedOctree *tree,
	const struct GeoPoint *p, double eps,
	GeoVertexVisitor visitor, void *ctx);


/* The following are higher order utility functions. They don't require
 * internals of GeoHashesOctree. */
typedef void GeoVertexDestructor(void *ptr, void *ctx);
GEO_EXPORT void GeoHODeleteDuplicates(struct GeoHashedOctree *tree, double eps,
	GeoVertexDestructor dtor, void *ctx);

#ifdef __cplusplus
}
#endif

#endif

