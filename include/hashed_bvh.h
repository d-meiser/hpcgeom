#ifndef HASHED_BVH_H
#define HASHED_BVH_H

#include <basic_types.h>
#include <spatial_hash.h>


#ifdef __cplusplus
extern "C" {
#endif

#define GEO_HASHED_BVH_MAX_DEPTH 10


struct GeoHashedBvhNode;

struct GeoHashedBvh {
	struct GeoBoundingBox *volumes;
	void **data;
	GeoSpatialHash *hashes;
	int size;
	int capacity;
	int level_begin[GEO_HASHED_BVH_MAX_DEPTH + 1];
	struct GeoBoundingBox bbox;
	struct GeoHashedBvhNode *root;
};

GEO_EXPORT void GeoHBInitialize(struct GeoHashedBvh *bvh,
	struct GeoBoundingBox bbox);
GEO_EXPORT void GeoHBDestroy(struct GeoHashedBvh *bvh);
GEO_EXPORT void GeoHBInsert(struct GeoHashedBvh *bvh, int n,
	struct GeoBoundingBox *volumes, void **data);
typedef int GeoVolumeVisitor(struct GeoBoundingBox *volumes, void **data, int i,
	void *ctx);
GEO_EXPORT void GeoHBVisitIntersectingVolumes(struct GeoHashedBvh *bvh,
	const struct GeoBoundingBox *volume,
	GeoVolumeVisitor visitor,
	void *ctx);


#ifdef __cplusplus
}
#endif

#endif
