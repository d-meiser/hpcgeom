#include <edge_set.h>
#include <string.h>
#include <stdlib.h>


#define SHORT_LIST_CAPACITY 64
static struct GeoEdge short_list_marker()
{
	static const struct GeoEdge end_marker = {{UINT32_MAX, UINT32_MAX}};
	return end_marker;
}

static int is_end(struct GeoEdge edge)
{
	if (edge.vertices[0] == UINT32_MAX && edge.vertices[1] == UINT32_MAX) {
		return 1;
	} else {
		return 0;
	}
}


void GeoESInitialize(struct GeoEdgeSet *es)
{
	memset(es, 0x0, sizeof(*es));
	es->capacity = SHORT_LIST_CAPACITY;
	es->large_list = malloc(es->capacity * sizeof(*es->large_list));
	// TODO: The short_list should really be a heap. Perhaps the large_list
	// as well?
	es->short_list = malloc(SHORT_LIST_CAPACITY * sizeof(*es->short_list));
	es->short_list[0] = short_list_marker();
}

void GeoESDestroy(struct GeoEdgeSet *es)
{
	free(es->large_list);
	free(es->short_list);
}

static void sort_vertices(GeoVertexId *v)
{
	if (v[1] < v[0]) {
		GeoVertexId tmp = v[0];
		v[0] = v[1];
		v[1] = tmp;
	}
}

static int is_end_marker(const struct GeoEdge *edge)
{
	if (UINT32_MAX == edge->vertices[0] &&
	    UINT32_MAX == edge->vertices[1]) {
		return 1;
	}
	return 0;
}

static int short_list_end(const struct GeoEdge* short_list, int n_max)
{
	int i = 0;
	while (i < n_max && !is_end_marker(short_list + i)) ++i;
	return i;
}

static GeoEdgeId compute_edge_id(const struct GeoEdge edge)
{
	return (uint64_t)edge.vertices[0] << 32 | (uint64_t)edge.vertices[1];
}

void insertion_sort(struct GeoEdge *edges, int n)
{
	for (int i = 1; i < n; ++i) {
		GeoEdgeId key = compute_edge_id(edges[i]);
		int j = i - 1;
		while (j >= 0 && compute_edge_id(edges[j]) > key) {
			edges[j + 1] = edges[j];
			--j;
		}
		edges[j + 1] = edges[i];
	}
}

void merge(struct GeoEdge *e1, int n1, struct GeoEdge *e2, int n2)
{
	int k = n1 + n2;
	--k;
	--n1;
	--n2;
	while (n1 > 0 && n2 > 0) {
		if (compute_edge_id(e1[n1]) > compute_edge_id(e2[n2])) {
			e1[k] = e1[n1];
			--n1;
		} else {
			e1[k] = e2[n2];
			--n2;
		}
		--k;
	}
	while (n1 > 0) {
		e1[k] = e1[n1];
		--n1;
	}
	while (n2 > 0) {
		e1[k] = e2[n2];
		--n2;
	}
}

static void flush_short_list(struct GeoEdge *short_list,
	struct GeoEdge **large_list, int *size, int *capacity)
{
	int n = 0;
	while (n < SHORT_LIST_CAPACITY && !is_end(short_list[n])) ++n;

	insertion_sort(short_list, n);

	int new_capacity = *capacity;
	static const double kGrowthFactor = 1.7;
	while (*size + n > new_capacity) new_capacity *= kGrowthFactor;
	if (new_capacity > *capacity) {
		*large_list = realloc(*large_list,
			new_capacity * sizeof(**large_list));
		*capacity = new_capacity;
	}

	merge(*large_list, *size, short_list, n);
	*size += n;

	short_list[0] = short_list_marker();
}

GeoEdgeId GeoESInsert(struct GeoEdgeSet *es, struct GeoEdge edge)
{
	sort_vertices(edge.vertices);
	int i = short_list_end(es->short_list, SHORT_LIST_CAPACITY);
	if (i == SHORT_LIST_CAPACITY) {
		flush_short_list(es->short_list,
			&es->large_list, &es->size, &es->capacity);
		i = 0;
	}
	es->short_list[i] = edge;
	if (i < SHORT_LIST_CAPACITY - 1) {
		es->short_list[i + 1] = short_list_marker();
	}
	return compute_edge_id(edge);
}

static int lower_bound(const struct GeoEdge *edge, int n, GeoEdgeId x)
{
	int l = 0;
	int h = n;
	while (l < h) {
		int mid = (l + h) / 2;
		if (x <= compute_edge_id(edge[mid])) {
			h = mid;
		} else {
			l = mid + 1;
		}
	}
	return l;
}

struct GeoEdge *GeoESGetEdge(struct GeoEdgeSet *es, GeoEdgeId id)
{
	int i = lower_bound(es->large_list, es->size, id);
	if (i != es->size) return &es->large_list[i];
	for (i = 0; i < SHORT_LIST_CAPACITY && is_end(es->short_list[i]); ++i) {
		if (id == compute_edge_id(es->short_list[i])) {
			return &es->short_list[i];
		}
	}
	return 0;
}

