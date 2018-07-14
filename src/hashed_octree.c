#include <hashed_octree.h>
#include <string.h>
#include <stdlib.h>
#include <qsort.h>


void GeoHOInitialize(struct GeoHashedOctree* tree, struct GeoBoundingBox b)
{
	memset(tree, 0, sizeof(*tree));
	GeoVAInitialize(&tree->vertices);
	tree->hashes = malloc(tree->vertices.capacity * sizeof(*tree->hashes));
	tree->bbox = b;
}

void GeoHODestroy(struct GeoHashedOctree* tree)
{
	GeoVADestroy(&tree->vertices);
	free(tree->hashes);
}

struct TaggedHash {
	uint32_t hash;
	uint32_t tag;
};
union BigHash {
	uint64_t hash;
	struct TaggedHash tagged_hash;
};

static void ComputeHashes(const struct GeoBoundingBox *b,
	const struct GeoVertexArray *va,
	int begin, int end,
	union BigHash *hashes)
{
	for (int i = 0; i < end - begin; ++i) {
		struct GeoPoint p = {
			va->x[begin + i],
			va->y[begin + i],
			va->z[begin + i]};
		hashes[i].tagged_hash.hash = GeoComputeHash(b, &p);
		hashes[i].tagged_hash.tag = i;
	}
}

static void merge(
	GeoSpatialHash **hashes_1, struct GeoVertexArray *va_1,
	const union BigHash *hashes_2,
	int begin, int end, const struct GeoVertexArray *va_2)
{
	int n1 =  va_1->size;
	int n2 = end - begin;

	struct GeoVertexArray temp_va;
	GeoVAInitialize(&temp_va);
	GeoVAResize(&temp_va, n1 + n2);
	GeoSpatialHash *temp_hashes = malloc((n1 + n2) * sizeof(*temp_hashes));

	int i = 0;
	int j = 0;
	int k = 0;
	while (i < n1 && j  < n2) {
		if ((*hashes_1)[i] < hashes_2[j].tagged_hash.hash) {
			temp_hashes[k] = (*hashes_1)[i];
			temp_va.x[k] = va_1->x[i];
			temp_va.y[k] = va_1->y[i];
			temp_va.z[k] = va_1->z[i];
			temp_va.ptrs[k] = va_1->ptrs[i];
			++i;
		} else {
			temp_hashes[k] = hashes_2[j].tagged_hash.hash;
			int m = begin + hashes_2[j].tagged_hash.tag;
			temp_va.x[k] = va_2->x[m];
			temp_va.y[k] = va_2->y[m];
			temp_va.z[k] = va_2->z[m];
			temp_va.ptrs[k] = va_2->ptrs[m];
			++j;
		}
		++k;
	}

	while (i < n1) {
		temp_hashes[k] = (*hashes_1)[i];
		temp_va.x[k] = va_1->x[i];
		temp_va.y[k] = va_1->y[i];
		temp_va.z[k] = va_1->z[i];
		temp_va.ptrs[k] = va_1->ptrs[i];
		++i;
		++k;
	}

	while (j < n2) {
		temp_hashes[k] = hashes_2[j].tagged_hash.hash;
		int m = begin + hashes_2[j].tagged_hash.tag;
		temp_va.x[k] = va_2->x[m];
		temp_va.y[k] = va_2->y[m];
		temp_va.z[k] = va_2->z[m];
		temp_va.ptrs[k] = va_2->ptrs[m];
		++j;
		++k;
	}

	free(*hashes_1);
	*hashes_1 = temp_hashes;
	GeoVASwap(&temp_va, va_1);
	GeoVADestroy(&temp_va);
}

void GeoHOInsert(struct GeoHashedOctree *tree,
	const struct GeoVertexArray *va, int begin, int end)
{
	int num_items = end - begin;
	if (num_items <= 0) return;
	union BigHash new_hashes[end - begin];
	ComputeHashes(&tree->bbox, va, begin, end, new_hashes);
	GeoQsort((uint64_t*)new_hashes, num_items);
	merge(&tree->hashes, &tree->vertices, new_hashes, begin, end, va);
}
