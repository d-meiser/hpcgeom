#include <hashed_octree.h>
#include <string.h>
#include <stdlib.h>
#include <qsort.h>
#include <math.h>
#include <assert.h>


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
	const struct GeoVertexArray *va,
	int begin, int end,
	uint64_t *hashes)
{
	for (int i = 0; i < end - begin; ++i) {
		struct GeoPoint p = {
			va->x[begin + i],
			va->y[begin + i],
			va->z[begin + i]};
		GeoSpatialHash hash = GeoComputeHash(b, &p);
		hashes[i] = BigHash(hash, i);
	}
}

#ifndef NDEBUG
static int hashes_are_sorted(GeoSpatialHash *h, int n)
{
	for (int i = 0; i < n - 1; ++i) {
		if (h[i] > h[i + 1]) return 0;
	}
	return 1;
}
#endif

static void merge(
	GeoSpatialHash **hashes_1, struct GeoVertexArray *va_1,
	const uint64_t *hashes_2,
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
		if ((*hashes_1)[i] < GetHash(hashes_2[j])) {
			temp_hashes[k] = (*hashes_1)[i];
			temp_va.x[k] = va_1->x[i];
			temp_va.y[k] = va_1->y[i];
			temp_va.z[k] = va_1->z[i];
			temp_va.ptrs[k] = va_1->ptrs[i];
			++i;
		} else {
			temp_hashes[k] = GetHash(hashes_2[j]);
			int m = begin + GetTag(hashes_2[j]);
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
		temp_hashes[k] = GetHash(hashes_2[j]);
		int m = begin + GetTag(hashes_2[j]);
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

	assert(hashes_are_sorted(*hashes_1, va_1->size));
}

void GeoHOInsert(struct GeoHashedOctree *tree,
	const struct GeoVertexArray *va, int begin, int end)
{
	int num_items = end - begin;
	if (num_items <= 0) return;
	uint64_t new_hashes[end - begin];
	ComputeHashes(&tree->bbox, va, begin, end, new_hashes);
	GeoQsort((uint64_t*)new_hashes, num_items);
	merge(&tree->hashes, &tree->vertices, new_hashes, begin, end, va);
}

struct NodeList {
	struct NodeList* next;
	GeoNodeKey node;
};
static void NodeListDelete(struct NodeList *list) {
	while (list) {
		struct NodeList *next = list->next;
		free(list);
		list = next;
	}
}
static struct NodeList *NodeListPush(struct NodeList *list, GeoNodeKey node) {
	struct NodeList *new_node = malloc(sizeof(*new_node));
	new_node->next = list;
	new_node->node = node;
	return new_node;
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

static double volume(const struct GeoBoundingBox* bbox)
{
	return
		(bbox->max.x - bbox->min.x) *
		(bbox->max.y - bbox->min.y) *
		(bbox->max.z - bbox->min.z);
}

static void find_overlapping_nodes(
	GeoNodeKey node, const struct GeoBoundingBox *bbox,
	const struct GeoBoundingBox *p_bbox, double eps_cubed,
	struct NodeList **node_list)
{
	if (boxes_overlap(p_bbox, bbox)) {
		// Keep this node if we have reached the finest level or
		// if the node is of comparable size to the bounding volume
		// of the point (eps_cubed). Note that the exact termination
		// criterion can be tuned. Choosing a tighter criterion leads to
		// more (but smaller) nodes and rejects more candidate vertices.
		// Choosing a looser criterion leads to fewer (but larger) nodes
		// and rejects fewer vertices outright. The correctness of the
		// algorithm is not affected.
		if (GeoNodeLevel(node) == GeoNodeMaxDepth() ||
		    volume(bbox) < 8 * eps_cubed) {
			*node_list = NodeListPush(*node_list, node);
		} else {
			GeoNodeKey children[8];
			GeoNodeComputeChildKeys(node, children);
			struct GeoBoundingBox child_boxes[8];
			GeoComputeChildBoxes(bbox, child_boxes);
			for (int i = 0; i < 8; ++i) {
				find_overlapping_nodes(children[i],
					&child_boxes[i], p_bbox, eps_cubed,
					node_list);
			}
		}
	}
}

static struct NodeList *find_visit_list(const struct GeoPoint *p, double eps,
	const struct GeoBoundingBox *bbox)
{
	struct GeoBoundingBox p_bbox = {
		{ p->x - eps, p->y - eps, p->z - eps },
		{ p->x + eps, p->y + eps, p->z + eps }};
	GeoNodeKey node = GeoNodeSmallestContaining(bbox, &p_bbox);
	struct GeoBoundingBox smallest_bbox = GeoNodeBox(node, bbox);
	struct NodeList *visit_list = 0;
	find_overlapping_nodes(node, &smallest_bbox, &p_bbox, eps * eps * eps,
		&visit_list);
	return visit_list;
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

static int vertex_is_near(int i, const struct GeoVertexArray *va,
	const struct GeoPoint *p, double eps)
{
	if ((fabs(p->x - va->x[i]) <= eps) &&
	    (fabs(p->y - va->y[i]) <= eps) &&
	    (fabs(p->z - va->z[i]) <= eps)) {
		return 1;
	} else {
		return 0;
	}
}

static int visit_node(GeoNodeKey node, struct GeoHashedOctree *tree,
	const struct GeoPoint* p, double eps,
	GeoVertexVisitor visitor, void *ctx)
{
	GeoSpatialHash begin = GeoNodeBegin(node);
	GeoSpatialHash end = GeoNodeEnd(node);
	int l = lower_bound(tree->hashes, tree->vertices.size, begin);
	int h = upper_bound(tree->hashes, tree->vertices.size, end);
	struct GeoVertexArray *va = &tree->vertices;
	for (int i = l; i != h; ++i) {
		if (vertex_is_near(i, va, p, eps)) {
			int cont = visitor(va, i, ctx);
			if (0 == cont) return cont;
		}
	}
	return 1;
}

void GeoHOVisitNearVertices(struct GeoHashedOctree *tree,
	const struct GeoPoint* p, double eps,
	GeoVertexVisitor visitor, void *ctx)
{
	struct NodeList *visit_list =
		find_visit_list(p, eps, &tree->bbox);
	for (struct NodeList *n = visit_list; n != 0; n = n->next) {
		int cont = visit_node(n->node, tree, p, eps, visitor, ctx);
		if (0 == cont) {
			NodeListDelete(visit_list);
			return;
		}
	}
	NodeListDelete(visit_list);
}


struct DedupCtx {
	int self;
	int *vertices_to_delete;
	int size;
	int capacity;
};

static void DedupCtxInitialize(struct DedupCtx *ctx)
{
	memset(ctx, 0, sizeof(*ctx));
	ctx->capacity = 16;
	ctx->vertices_to_delete = malloc(ctx->capacity * sizeof(int));
}

static void DedupCtxDestroy(struct DedupCtx *ctx)
{
	free(ctx->vertices_to_delete);
	memset(ctx, 0, sizeof(*ctx));
}

static void DedupCtxPushBack(struct DedupCtx *ctx, int i)
{
	if (ctx->size == ctx->capacity) {
		static const double growth_factor = 1.7;
		ctx->capacity *= growth_factor;
		ctx->vertices_to_delete = realloc(ctx->vertices_to_delete,
			ctx->capacity * sizeof(int));
	}
	ctx->vertices_to_delete[ctx->size] = i;
	++ctx->size;
}

static int DedupVisitor(struct GeoVertexArray* va, int i, void *ctx)
{
	(void)va;
	struct DedupCtx *dedup_ctx = ctx;
	if (i < dedup_ctx->self) {
		DedupCtxPushBack(dedup_ctx, dedup_ctx->self);
	        return 0;
	}
	return 1;
}

void GeoHODeleteDuplicates(struct GeoHashedOctree *tree, double eps,
	GeoVertexDestructor dtor, void *ctx)
{
	struct DedupCtx dedup_ctx;
	DedupCtxInitialize(&dedup_ctx);
	for (int i = 0; i < tree->vertices.size; ++i) {
		dedup_ctx.self = i;
		struct GeoPoint p = {
			tree->vertices.x[i],
			tree->vertices.y[i],
			tree->vertices.z[i]};
		GeoHOVisitNearVertices(tree, &p, eps, DedupVisitor, &dedup_ctx);
	}
	char *deleted = calloc(tree->vertices.size, sizeof(*deleted));
	for (int i = 0; i < dedup_ctx.size; ++i) {
		int ii = dedup_ctx.vertices_to_delete[i];
		dtor(tree->vertices.ptrs[ii], ctx);
		tree->vertices.ptrs[ii] = 0;
		deleted[ii] = 1;
	}
	int j = 0;
	for (int i = 0; i < tree->vertices.size; ++i) {
		if (0 == deleted[i]) {
			tree->vertices.x[j] = tree->vertices.x[i];
			tree->vertices.y[j] = tree->vertices.y[i];
			tree->vertices.z[j] = tree->vertices.z[i];
			tree->vertices.ptrs[j] = tree->vertices.ptrs[i];
			++j;
		}
	}
	tree->vertices.size = j;
	free(deleted);
	DedupCtxDestroy(&dedup_ctx);
}

