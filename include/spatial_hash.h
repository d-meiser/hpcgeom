#ifndef SPATIAL_HASH_H
#define SPATIAL_HASH_H

#include <stdint.h>
#include <basic_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t GeoSpatialHash;
GeoSpatialHash GeoComputeHash(const struct GeoBoundingBox* b,
	const struct GeoPoint* p);

typedef uint32_t GeoNodeKey;
GeoNodeKey GeoNodeRoot();
int GeoNodeMaxDepth();
void GeoNodeComputeChildKeys(GeoNodeKey key, GeoNodeKey *child_keys);
int GeoNodeValidKey(GeoNodeKey key);
int GeoNodeLevel(GeoNodeKey key);
GeoNodeKey GeoNodeParent(GeoNodeKey key);
GeoSpatialHash GeoNodeBegin(GeoNodeKey key);
GeoSpatialHash GeoNodeEnd(GeoNodeKey key);
void GeoNodePrint(GeoNodeKey key);
struct GeoBoundingBox GeoNodeBox(GeoNodeKey key,
	const struct GeoBoundingBox *bbox);
struct GeoBoundingBox ComputeChildBox(
	const struct GeoBoundingBox *bbox, int octant);
GeoNodeKey GeoNodeSmallestContaining(const struct GeoBoundingBox* root_box,
	const struct GeoBoundingBox *b);

#ifdef __cplusplus
}
#endif

#endif
