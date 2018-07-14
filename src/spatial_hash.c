#include <spatial_hash.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>


// We use 32 bit keys. That is large enough for 2**10 buckets
// along each dimension.
#define BITS_PER_DIM 10
#define NUM_LEAF_BUCKETS (1u << BITS_PER_DIM)

static GeoSpatialHash ComputeBucket(double min, double max,
	double pos, GeoSpatialHash num_buckets)
{
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

static void MortonDecode_32(uint32_t code, int bits_per_dim,
	uint32_t *a, uint32_t *b, uint32_t *c)
{
	// TODO: Optimize decode using magic bits.
	*a = 0u;
	*b = 0u;
	*c = 0u;
	for (int i = 0; i < bits_per_dim; ++i) {
		if ((code >> 0) & (1u << (3 * i))) *a += 1u << i;
		if ((code >> 1) & (1u << (3 * i))) *b += 1u << i;
		if ((code >> 2) & (1u << (3 * i))) *c += 1u << i;
	}
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


GeoNodeKey GeoNodeRoot()
{
	return 1u;
}

int GeoNodeMaxDepth() {
	return BITS_PER_DIM;
}

struct GeoBoundingBox GeoNodeBox(GeoNodeKey key,
	const struct GeoBoundingBox *bbox)
{
	uint32_t a, b, c;
	int level = GeoNodeLevel(key);
	MortonDecode_32(key, level, &a, &b, &c);
	double dx = (bbox->max.x - bbox->min.x) / (1u << level);
	double dy = (bbox->max.y - bbox->min.y) / (1u << level);
	double dz = (bbox->max.z - bbox->min.z) / (1u << level);
	struct GeoBoundingBox this_box = {{
		bbox->min.x + a * dx,
		bbox->min.y + b * dy,
		bbox->min.z + c * dz
		}, {
		bbox->min.x + (a + 1) * dx,
		bbox->min.y + (b + 1) * dy,
		bbox->min.z + (c + 1) * dz
		}};
	return this_box;
}

void GeoNodeComputeChildKeys(GeoNodeKey key, GeoNodeKey* child_keys)
{
	GeoNodeKey first_child = key << 3;
	for (int i = 0; i < 8; ++i) {
		child_keys[i] = first_child + i;
	}
}

int GeoNodeValidKey(GeoNodeKey key)
{
	if (key & (1u << (BITS_PER_DIM * 3 + 1))) return 0;
	GeoNodeKey m = 1u << (BITS_PER_DIM * 3);
	while (m > 0) {
		if (key & m) return 1;
		for (int i = 0; i < 3; ++i, m >>= 1) {
			if (key & m) return 0;
		}
	}
	return 0;
}

int GeoNodeLevel(GeoNodeKey key)
{
	int level = BITS_PER_DIM;
	while (level > 0) {
		if (key & (1u << (level * 3))) return level;
		--level;
	}
	return level;
}

GeoNodeKey GeoNodeParent(GeoNodeKey key)
{
	return key >> 3;
}

GeoSpatialHash GeoNodeBegin(GeoNodeKey key)
{
	int level = GeoNodeLevel(key);
	GeoSpatialHash begin = key ^ (1u << (3 * level));
	begin <<= 3 * (BITS_PER_DIM - level);
	return begin;
}

GeoSpatialHash GeoNodeEnd(GeoNodeKey key)
{
	int level = GeoNodeLevel(key);
	GeoSpatialHash end = key ^ (1u << (3 * level));
	++end;
	end <<= 3 * (BITS_PER_DIM - level);
	return end;
}

void GeoNodePrint(GeoNodeKey key)
{
	for (int i = 31; i >= 0; --i) {
		if (key & (1u << i)) {
			printf("1");
		} else {
			printf("0");
		}
	}
}

GeoNodeKey GeoNodeSmallestContaining(const struct GeoBoundingBox* root_box,
	const struct GeoBoundingBox *b)
{
	GeoSpatialHash min = GeoComputeHash(root_box, &b->min);
	GeoSpatialHash max = GeoComputeHash(root_box, &b->max);
	int level = GeoNodeMaxDepth();
	while (min != max) {
		min >>= 3;
		max >>= 3;
		--level;
	}
	return min | (1u << (3 * level));
}
