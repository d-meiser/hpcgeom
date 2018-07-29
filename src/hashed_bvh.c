#include <hashed_bvh.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <qsort.h>
#include <spatial_hash.h>


struct GeoHashedBvhNode {
	int size;
	struct GeoHashedBvhNode *child[8];
};


static void GeoHBNInitialize(struct GeoHashedBvhNode *node)
{
	memset(node, 0, sizeof(*node));
}

static void GeoHBNDestroy(struct GeoHashedBvhNode *node)
{
	if (0 == node) return;
	for (int i = 0; i < 8; ++i) {
		struct GeoHashedBvhNode *child = node->child[i];
		GeoHBNDestroy(child);
		free(child);
	}
}

static struct GeoHashedBvhNode *GeoHBNNew()
{
	struct GeoHashedBvhNode *node = malloc(sizeof(*node));
	GeoHBNInitialize(node);
	return node;
}

static void GeoHBNDelete(struct GeoHashedBvhNode *node)
{
	GeoHBNDestroy(node);
	free(node);
}

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
	GeoHBNDelete(bvh->root);
	free(bvh->volumes);
	free(bvh->data);
	free(bvh->hashes);
}

static uint64_t BigHash(uint32_t hash, uint32_t tag)
{
	// Store the hash in the higher order bits so we sort by that.
	return ((uint64_t)hash << 32) | (uint64_t)tag;
};

static uint32_t GetHash(uint64_t big_hash)
{
	return (uint32_t)(0x00000000FFFFFFFFull & (big_hash >> 32));
}

static uint32_t GetTag(uint64_t big_hash)
{
	return (uint32_t)(0x00000000FFFFFFFFull & big_hash);
}


static void ComputeHashes(const struct GeoBoundingBox *b,
	const struct GeoBoundingBox *boxes,
	int n,
	uint64_t *hashes)
{
	for (int i = 0; i < n; ++i) {
		GeoNodeKey hash = GeoNodeSmallestContaining(b, &boxes[i]);
		hashes[i] = BigHash(hash, i);
	}
}

#ifndef NDEBUG
static int hashes_are_sorted(GeoNodeKey *h, int n)
{
	for (int i = 0; i < n - 1; ++i) {
		if (h[i] > h[i + 1]) return 0;
	}
	return 1;
}
#endif

static void merge(
	struct GeoHashedBvh *merged_bvh,
	struct GeoHashedBvh *bvh,
	int n,
	const uint64_t *hashes,
	struct GeoBoundingBox *volumes,
	void **data)
{
	int n1 = bvh->size;
	int n2 = n;

	const GeoNodeKey *hashes1 = bvh->hashes;
	const uint64_t *hashes2 = hashes;
	GeoNodeKey *hashes_merged = merged_bvh->hashes;

	struct GeoBoundingBox *volumes1 = bvh->volumes;
	struct GeoBoundingBox *volumes2 = volumes;
	struct GeoBoundingBox *volumes_merged = merged_bvh->volumes;

	void **data1 = bvh->data;
	void **data2 = data;
	void **data_merged = merged_bvh->data;

	int i = 0;
	int j = 0;
	int k = 0;

	while (i < n1 && j < n2) {
		if (hashes1[i] < GetHash(hashes2[j])) {
			hashes_merged[k] = hashes1[i];
			volumes_merged[k] = volumes1[i];
			data_merged[k] = data1[i];
			++i;
		} else {
			hashes_merged[k] = GetHash(hashes2[j]);
			uint32_t m = GetTag(hashes2[j]);
			volumes_merged[k] = volumes2[m];
			data_merged[k] = data2[m];
			++j;
		}
		++k;
	}

	while (i < n1) {
		hashes_merged[k] = hashes1[i];
		volumes_merged[k] = volumes1[i];
		data_merged[k] = data1[i];
		++i;
		++k;
	}

	while (j < n2) {
		hashes_merged[k] = GetHash(hashes2[j]);
		uint32_t m = GetTag(hashes2[j]);
		volumes_merged[k] = volumes2[m];
		data_merged[k] = data2[m];
		++j;
		++k;
	}

	assert(hashes_are_sorted(hashes_merged, n1 + n2));
}

static void reset_sizes(struct GeoHashedBvhNode *node)
{
	node->size = 0;
	for (int i = 0; i < 8; ++i) {
		struct GeoHashedBvhNode *child = node->child[i];
		if (child) reset_sizes(child);
	}
}

static void add_entity(struct GeoHashedBvhNode **node, int level,
	GeoNodeKey hash)
{
	if (0 == *node) *node = GeoHBNNew();
	++(*node)->size;
	if (level > 0) {
		int i = (hash >> (3 * level)) & 0x7;
		add_entity(&(*node)->child[i], level - 1, hash);
	}
}

static void recompute_sizes(struct GeoHashedBvh *bvh)
{
	if (bvh->root) {
		reset_sizes(bvh->root);
	}
	for (int i = 0; i < bvh->level_begin[GEO_HASHED_BVH_MAX_DEPTH]; ++i) {
		GeoNodeKey hash = bvh->hashes[i];
		add_entity(&bvh->root, GeoNodeLevel(hash), hash);
	}
}

static uint64_t lower_bound_64(uint64_t* arr, uint64_t n, uint64_t x)
{
	uint64_t l = 0;
	uint64_t h = n;
	while (l < h) {
		uint64_t mid = (l + h) / 2;
		if (x <= arr[mid]) {
			h = mid;
		} else {
			l = mid + 1;
		}
	}
	return l;
}

void GeoHBInsert(struct GeoHashedBvh *bvh, int n,
	struct GeoBoundingBox *volumes, void **data)
{
	// Compute hashes
	uint64_t *new_hashes;
	new_hashes = malloc(n * sizeof(*new_hashes));
	ComputeHashes(&bvh->bbox, volumes, n, new_hashes);

	GeoQsort(new_hashes, n);

	// Find level partitioning
	int level_begin[GEO_HASHED_BVH_MAX_DEPTH + 1];
	level_begin[0] = 0;
	// TODO: This could be optimized by recursively partitioning
	// the hashes.
	for (int i = 0; i < GEO_HASHED_BVH_MAX_DEPTH; ++i) {
		level_begin[i + 1] = level_begin[i] +
			lower_bound_64(
				new_hashes + level_begin[i],
				n - level_begin[i],
				BigHash(0x1u << (3 * (i + 1)), 0x0u));
	}
	assert(level_begin[GEO_HASHED_BVH_MAX_DEPTH] == n);

	// Merge the sorted hashes
	struct GeoHashedBvh merged_bvh;
	GeoHBInitialize(&merged_bvh, bvh->bbox);
	reserve_space(&merged_bvh, bvh->size + n);
	merged_bvh.root = bvh->root;
	bvh->root = 0;
	merge(&merged_bvh, bvh, n, new_hashes, volumes, data);

	// Update the level pointers
	for (int i = 0; i < GEO_HASHED_BVH_MAX_DEPTH + 1; ++i) {
		merged_bvh.level_begin[i] =
			bvh->level_begin[i] + level_begin[i];
	}

	// Swap data into bvh and cleanup
	GeoHBDestroy(bvh);
	*bvh = merged_bvh;
	free(new_hashes);

	// Update the size information
	recompute_sizes(bvh);
}

static uint32_t lower_bound(uint32_t* arr, uint32_t n, uint32_t x)
{
	uint32_t l = 0;
	uint32_t h = n;
	while (l < h) {
		uint32_t mid = (l + h) / 2;
		if (x <= arr[mid]) {
			h = mid;
		} else {
			l = mid + 1;
		}
	}
	return l;
}

static uint32_t upper_bound(uint32_t* arr, uint32_t n, uint32_t x)
{
	uint32_t l = 0;
	uint32_t h = n;
	while (l < h) {
		uint32_t mid = (l + h) / 2;
		if (x >= arr[mid]) {
			l = mid + 1;
		} else {
			h = mid;
		}
	}
	return l;
}

static int boxes_overlap(
	const struct GeoBoundingBox* a, const struct GeoBoundingBox* b)
{
	if (a->max.x >= b->min.x && b->max.x >= a->min.x &&
	    a->max.y >= b->min.y && b->max.y >= a->min.y &&
	    a->max.z >= b->min.z && b->max.z >= a->min.z) {
		return 1;
	} else {
		return 0;
	}
}

static int visit_node(
	GeoNodeKey node,
	struct GeoHashedBvhNode *tree_node,
	const struct GeoBoundingBox *my_bbox,
	struct GeoHashedBvh *bvh,
	const struct GeoBoundingBox *volume,
	GeoVolumeVisitor visitor,
	void *ctx)
{
	// Bail early if the subtree starting at this node is empty
	if (!tree_node || tree_node->size == 0) return 1;

	// Visit own volumes
	GeoNodeKey begin = GeoNodeBegin(node);
	GeoNodeKey end = GeoNodeEnd(node);
	int level = GeoNodeLevel(node);
	begin |= (0x1u << (3 * level));
	end |= (0x1u << (3 * level));
	int n = bvh->level_begin[level + 1] - bvh->level_begin[level];
	int l = lower_bound(bvh->hashes + bvh->level_begin[level], n, begin);
	int h = upper_bound(bvh->hashes + bvh->level_begin[level], n, end);
	for (int i = l; i < h; ++i) {
		if (boxes_overlap(&bvh->volumes[i], volume)) {
			int cont = visitor(bvh->volumes, bvh->data, i, ctx);
			if (cont == 0) return 0;
		}
	}
	if (level == GEO_HASHED_BVH_MAX_DEPTH - 1) return 1;

	// Visit children
	GeoNodeKey children[8];
	GeoNodeComputeChildKeys(node, children);
	struct GeoBoundingBox child_boxes[8];
	GeoComputeChildBoxes(my_bbox, child_boxes);
	for (int i = 0; i < 8; ++i) {
		if (boxes_overlap(&child_boxes[i], volume)) {
			int cont = visit_node(
				children[i], tree_node->child[i],
				&child_boxes[i],
				bvh, volume, visitor, ctx);
			if (cont == 0) return 0;
		}
	}
	return 1;
}


void GeoHBVisitIntersectingVolumes(struct GeoHashedBvh *bvh,
	const struct GeoBoundingBox *volume,
	GeoVolumeVisitor visitor,
	void *ctx)
{
	visit_node(GeoNodeRoot(), bvh->root, &bvh->bbox, bvh, volume, visitor,
		ctx);
}


