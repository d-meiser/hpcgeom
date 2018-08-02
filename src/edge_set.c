#include <edge_set.h>
#include <string.h>
#include <stdlib.h>


#define SHORT_LIST_CAPACITY 64


void GeoESInitialize(struct GeoEdgeSet *es)
{
	memset(es, 0x0, sizeof(*es));
	es->capacity = SHORT_LIST_CAPACITY;
	es->large_list = malloc(es->capacity * sizeof(*es->large_list));
	es->short_list = malloc(SHORT_LIST_CAPACITY * sizeof(*es->short_list));
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

static struct GeoEdge short_list_marker()
{
	static const struct GeoEdge end_marker = {{UINT32_MAX, UINT32_MAX}};
	return end_marker;
}

static GeoEdgeId compute_edge_id(const struct GeoEdge *edge)
{
	return (uint64_t)edge->vertices[0] << 32 | (uint64_t)edge->vertices[1];
}

GeoEdgeId GeoESInsert(struct GeoEdgeSet *es, struct GeoEdge edge)
{
	sort_vertices(edge.vertices);
	int i = short_list_end(es->short_list, SHORT_LIST_CAPACITY);
	if (i == SHORT_LIST_CAPACITY) {
		// TODO: Out of space in the short list. We have to move stuff
		// over to the long list.
	}
	es->short_list[i] = edge;
	if (i < SHORT_LIST_CAPACITY - 1) {
		es->short_list[i + 1] = short_list_marker();
	}
	return compute_edge_id(&edge);
}
