#ifndef HASHED_OCTREE_H
#define HASHED_OCTREE_H

#include <basic_types.h>
#include <spatial_hash.h>
#include <vertex_array.h>


#ifdef __cplusplus
extern "C" {
#endif


struct GeoHashedOctree
{
	struct GeoVertexArray vertices;
	GeoSpatialHash *hashes;
	struct GeoBoundingBox bbox;
};

void GeoHOInitialize(struct GeoHashedOctree *tree, struct GeoBoundingBox b);
void GeoHODestroy(struct GeoHashedOctree *tree);

/** Insert items into tree
 *
 * @param tree The tree into which to insert.
 * @param va   The vertex array out of which to insert.
 * @param begin Index of first vertex to insert.
 * @param end  One past index of last index to insert.
 */
void GeoHOInsert(struct GeoHashedOctree *tree,
	const struct GeoVertexArray *va, int begin, int end);

typedef int GeoVertexVisitor(struct GeoVertexArray *va, int i, void *ctx);
void GeoHOVisitNearVertices(struct GeoHashedOctree *tree,
	const struct GeoPoint *p, double eps,
	GeoVertexVisitor visitor, void *ctx);

typedef void GeoVertexDestructor(void *ptr, void *ctx);
void GeoHODeleteDuplicates(struct GeoHashedOctree *tree, double eps,
	GeoVertexDestructor dtor, void *ctx);

#ifdef __cplusplus
}
#endif

#endif

