#ifndef HASHED_BVH_H
#define HASHED_BVH_H

#include <basic_types.h>
#include <spatial_hash.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoHashedBvh {
	struct GeoBoundingBox *volumes;
	void *data;
	GeoSpatialHash *hashes;
	int size;
	int capacity;
	int levels[11];
	struct GeoBoundingBox bbox;
};

GEO_EXPORT void GeoHBInitialize(struct GeoHashedBvh *bvh,
	struct GeoBoundingBox bbox);
GEO_EXPORT void GeoHBDestroy(struct GeoHashedBvh *bvh);
GEO_EXPORT void GeoHBInsert(struct GeoHashedBvh *bvh, int n,
	const struct GeoBoundingBox *volumes, const void *data);
typedef int GeoVolumeVisitor(struct GeoBoundingBox *volumes, void *data, int i,
	void *ctx);
GEO_EXPORT void GeoHBVisitIntersectingVolumes(struct GeoHashedBvh *bvh,
	const struct GeoBoundingBox *volume,
	GeoVolumeVisitor visitor,
	void *ctx);


#ifdef __cplusplus
}
#endif

#endif
