#ifndef SPATIAL_HASH_H
#define SPATIAL_HASH_H


typedef uint32_t GeoSpatialHash;
GeoSpatialHash GeoComputeHash(const struct GeoBoundingBox* b,
	const struct GeoPoint* p);

#endif
