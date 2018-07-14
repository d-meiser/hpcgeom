#ifndef HASHED_OCTREE_H
#define HASHED_OCTREE_H

#include <basic_types.h>
#include <spatial_hash.h>
#include <vertex_array.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoHashedOctreeNode;

struct GeoHashedOctree
{
	struct GeoVertexArray vertices;
	GeoSpatialHash *hashes;
	struct GeoBoundingBox bbox;
	struct GeoHashedOctreeNode *root;
};

void GeoHOInitialize(struct GeoHashedOctree* tree, struct GeoBoundingBox b);
void GeoHODestroy(struct GeoHashedOctree* tree);

/** Insert items into tree
 *
 * @param tree The tree into which to insert.
 * @param va   The vertex array out of which to insert.
 * @param begin Index of first vertex to insert.
 * @param end  One past index of last index to insert.
 */
void GeoHOInsert(struct GeoHashedOctree* tree,
	const struct GeoVertexArray* va, int begin, int end);

/** Uniformly grow bounding box until it fully encloses the new
 *  bounding volume.*/
void GeoHOGrowBoundingBox(struct GeoHashedOctree *tree,
	const struct GeoBoundingBox* new_bb);

typedef void GeoVertexVisitor(double x, double y, double z, void* ptr, void* ctx);
void GeoHOVisitNearVertices(struct GeoHashedOctree *tree,
	const struct GeoPoint* p, double eps,
	GeoVertexVisitor visitor, void* ctx);

#ifdef __cplusplus
}
#endif

#endif

