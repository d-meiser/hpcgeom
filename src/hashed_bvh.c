#include <hashed_bvh.h>
#include <string.h>
#include <stdlib.h>


static void reserve_space(struct GeoHashedBvh *bvh, int capacity)
{
	if (capacity > bvh->capacity) {
		bvh->volumes = realloc(bvh->volumes,
			capacity * sizeof(*bvh->volumes));
		bvh->data = realloc(bvh->data, capacity * sizeof(*bvh->data));
		bvh->hashes = realloc(bvh->hashes,
			capacity * sizeof(*bvh->hashes));
		bvh->capacity = capacity;
	}
}


void GeoHBInitialize(struct GeoHashedBvh *bvh, struct GeoBoundingBox bbox)
{
	memset(bvh, 0, sizeof(*bvh));
	static const int initial_capacity = 32;
	reserve_space(bvh, initial_capacity);
	bvh->bbox = bbox;
}

void GeoHBDestroy(struct GeoHashedBvh *bvh)
{
	free(bvh->volumes);
	free(bvh->data);
	free(bvh->hashes);
}

