#ifndef SPATIAL_HASH_H
#define SPATIAL_HASH_H

#include <stdint.h>
#include <basic_types.h>
#include <geo_export.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t GeoSpatialHash;
GEO_EXPORT GeoSpatialHash GeoComputeHash(const struct GeoBoundingBox* b,
	const struct GeoPoint* p);

typedef uint32_t GeoNodeKey;
GEO_EXPORT GeoNodeKey GeoNodeRoot();
GEO_EXPORT int GeoNodeMaxDepth();
GEO_EXPORT void GeoNodeComputeChildKeys(GeoNodeKey key, GeoNodeKey *child_keys);
GEO_EXPORT void GeoComputeChildBoxes(
	const struct GeoBoundingBox *bbox, struct GeoBoundingBox *child_boxes);
GEO_EXPORT int GeoNodeValidKey(GeoNodeKey key);
GEO_EXPORT int GeoNodeLevel(GeoNodeKey key);
GEO_EXPORT GeoNodeKey GeoNodeParent(GeoNodeKey key);
GEO_EXPORT GeoSpatialHash GeoNodeBegin(GeoNodeKey key);
GEO_EXPORT GeoSpatialHash GeoNodeEnd(GeoNodeKey key);
GEO_EXPORT void GeoNodePrint(GeoNodeKey key);
GEO_EXPORT struct GeoBoundingBox GeoNodeBox(GeoNodeKey key,
	const struct GeoBoundingBox *bbox);
GEO_EXPORT GeoNodeKey GeoNodeSmallestContaining(const struct GeoBoundingBox* root_box,
	const struct GeoBoundingBox *b);

#ifdef __cplusplus
}
#endif

#endif
