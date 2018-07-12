#include <spatial_hash.h>
#include <math.h>
#include <assert.h>


// We use 32 bit keys. That is large enough for 2**10 buckets
// along each dimension.
#define BITS_PER_DIM 10
#define NUM_LEAF_BUCKETS (1u << BITS_PER_DIM)

static GeoSpatialHash ComputeBucket(double min, double max,
				    double pos, GeoSpatialHash num_buckets) {
	assert(max > min);
	double folded_pos = fmod(pos - min, max - min);
	if (folded_pos < 0) {
		folded_pos += max - min;
	}
	int bucket = num_buckets * folded_pos / (max - min);
	assert(bucket >= 0);
	assert((uint32_t)bucket < num_buckets);
	return bucket;
}


static uint32_t Part1By2_32(uint32_t a)
{
	a &= 0x000003ff;                  // a = ---- ---- ---- ---- ---- --98 7654 3210
	a = (a ^ (a << 16)) & 0xff0000ff; // a = ---- --98 ---- ---- ---- ---- 7654 3210
	a = (a ^ (a <<  8)) & 0x0300f00f; // a = ---- --98 ---- ---- 7654 ---- ---- 3210
	a = (a ^ (a <<  4)) & 0x030c30c3; // a = ---- --98 ---- 76-- --54 ---- 32-- --10
	a = (a ^ (a <<  2)) & 0x09249249; // a = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
	return a;
}

static uint32_t MortonEncode_32(uint32_t a, uint32_t b, uint32_t c)
{
	return Part1By2_32(a) + (Part1By2_32(b) << 1) + (Part1By2_32(c) << 2);
}

GeoSpatialHash GeoComputeHash(const struct GeoBoundingBox* bbox,
	const struct GeoPoint* p)
{
	GeoSpatialHash a, b, c;
	a = ComputeBucket(bbox->min.x, bbox->max.x, p->x, NUM_LEAF_BUCKETS);
	b = ComputeBucket(bbox->min.y, bbox->max.y, p->y, NUM_LEAF_BUCKETS);
	c = ComputeBucket(bbox->min.z, bbox->max.z, p->z, NUM_LEAF_BUCKETS);
	return MortonEncode_32(a, b, c);
}


