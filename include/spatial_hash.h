#ifndef SPATIAL_HASH_H
#define SPATIAL_HASH_H


namespace hpcgeo {

typedef uint32_t SpatialHash;
SpatialHash ComputeHash(const BoundingBox& b, const Point& p);

} // namespace hpcgeo

#endif
