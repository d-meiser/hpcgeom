#include <hashed_octree.h>
#include <string.h>
#include <stdlib.h>
#include <qsort.h>
#include <math.h>


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

static int box_contains(
	const struct GeoBoundingBox* a, const struct GeoBoundingBox* b)
{
	if (a->min.x <= b->min.x && a->max.x >= b->max.x &&
	    a->min.y <= b->min.y && a->max.y >= b->max.y &&
	    a->min.z <= b->min.z && a->max.z >= b->max.z) {
		return 1;
	} else {
		return 0;
	}
}

void find_overlapping_nodes(
	GeoNodeKey node, const struct GeoBoundingBox *bbox,
	const struct GeoBoundingBox *p_bbox,
	struct NodeList **node_list)
{
	struct GeoBoundingBox this_box = GeoNodeBox(node, bbox);
	if (boxes_overlap(p_bbox, &this_box)) {
		// We recurse until we can't subdivide further or
		// until p_bbox completely contains the node's bounding box.
		// In the latter case all children also overlap.
		if (GeoNodeLevel(node) < GeoNodeMaxDepth() &&
		    0 != box_contains(p_bbox, &this_box)) {
			GeoNodeKey children[8];
			GeoNodeComputeChildKeys(node, children);
			for (int i = 0; i < 8; ++i) {
				find_overlapping_nodes(children[i],
					bbox, p_bbox, node_list);
			}
		} else {
			*node_list = NodeListPush(*node_list, node);
		}
	}
}


static struct NodeList *find_visit_list(const struct GeoPoint *p, double eps,
	const struct GeoBoundingBox *bbox)
{
	struct GeoBoundingBox p_bbox = {
		{ p->x - 0.5 * eps, p->y - 0.5 * eps, p->z - 0.5 * eps },
		{ p->x + 0.5 * eps, p->y + 0.5 * eps, p->z + 0.5 * eps }};
	// TODO: There is a better starting point that can be found by finding
	// the smallest node that fully contains the point's bounding box.
	// This can be found by coarsening the hashes for min and max of
	// the points bbox until they are identical. This is an optimization
	// worth exploiting.
	GeoNodeKey node = GeoNodeRoot();
	struct NodeList *visit_list = 0;
	find_overlapping_nodes(node, bbox, &p_bbox, &visit_list);
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
	if (fabs(p->x - va->x[i]) <= eps &&
	    fabs(p->y - va->y[i]) <= eps &&
	    fabs(p->z - va->z[i]) <= eps) {
		return 1;
	} else {
		return 0;
	}
}

static void visit_node(GeoNodeKey node, struct GeoHashedOctree *tree,
	const struct GeoPoint* p, double eps,
	GeoVertexVisitor visitor, void *ctx)
{
	GeoSpatialHash begin = GeoNodeBegin(node);
	GeoSpatialHash end = GeoNodeEnd(node);
	int l = lower_bound(tree->hashes, tree->vertices.size, begin);
	int h = upper_bound(tree->hashes, tree->vertices.size, end);
	const struct GeoVertexArray *va = &tree->vertices;
	for (int i = l; i != h; ++i) {
		if (vertex_is_near(i, va, p, eps)) {
			visitor(va->x[i], va->y[i], va->z[i], va->ptrs[i], ctx);
		}
	}
}

void GeoHOVisitNearVertices(struct GeoHashedOctree *tree,
	const struct GeoPoint* p, double eps,
	GeoVertexVisitor visitor, void *ctx)
{
	struct NodeList *visit_list =
		find_visit_list(p, eps, &tree->bbox);
	for (struct NodeList *n = visit_list; n != 0; n = n->next) {
		visit_node(n->node, tree, p, eps, visitor, ctx);
	}
	NodeListDelete(visit_list);
}
