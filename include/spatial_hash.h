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

#ifdef __cplusplus
}
#endif

#endif
