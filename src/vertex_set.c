#include <vertex_set.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


void GeoVSInitialize(struct GeoVertexSet *vs, struct GeoBoundingBox bbox,
	double epsilon)
{
	GeoHOInitialize(&vs->octree, bbox);
	GeoVAInitialize(&vs->short_list);
	vs->size = 0;
	vs->capacity = 32;
	vs->vertex_data = malloc(vs->capacity * sizeof(*vs->vertex_data));
	vs->next_id = 0;
	vs->epsilon = epsilon;
	GeoHTInitialize(&vs->id_map);
}

void GeoVSDestroy(struct GeoVertexSet *vs)
{
	GeoVADestroy(&vs->short_list);
	GeoHODestroy(&vs->octree);
	for (unsigned i = 0; i < vs->size; ++i) {
		GeoVDDestroy(&vs->vertex_data[i]);
	}
	free(vs->vertex_data);
	GeoHTDestroy(&vs->id_map);
}

struct PointLocatorCtx
{
	int location;
};

int locate_point(struct GeoVertexArray *va, int i, void *ctx)
{
	(void)va;
	// Stop searching as soon as we encounter the first occurance of the
	// point in the tree. Save the index of the point.
	struct PointLocatorCtx *locator_ctx = ctx;
	locator_ctx->location = i;
	return 0;
}

static int find_point_in_tree(struct GeoHashedOctree *t,
	const struct GeoPoint* p, double eps)
{
	struct PointLocatorCtx ctx;
	ctx.location = -1;
	GeoHOVisitNearVertices(t, p, eps, locate_point, &ctx);
	return ctx.location;
}

static int find_point_in_array(struct GeoVertexArray *va,
	const struct GeoPoint *p, double epsilon)
{
	int i = 0;
	for (; i < va->size; ++i) {
		if ((fabs(va->x[i] - p->x) < epsilon) &&
		    (fabs(va->y[i] - p->y) < epsilon) &&
		    (fabs(va->z[i] - p->z) < epsilon)) {
			break;
		}
	}
	return i;
}

void push_back_vertex(struct GeoVertexArray *va, struct GeoVertex v)
{
	int end = va->size;
	GeoVAResize(va, va->size + 1);
	va->x[end] = v.p.x;
	va->y[end] = v.p.x;
	va->z[end] = v.p.x;
	va->ptrs[end] = v.ptr;
}

#define LOC_IN_SHORT_TABLE ((uint32_t)0x1u << 31)
/*
static int in_short_table(uint32_t location)
{
	return location & LOC_IN_SHORT_TABLE;
}
*/

struct GeoVertexData *GeoVSInsert(struct GeoVertexSet *vs,
	const struct GeoPoint p, GeoId *id)
{
	// First look for the point in the octree.
	int point_location =
		find_point_in_tree(&vs->octree, &p, vs->epsilon);
	if (point_location >= 0) {
		struct GeoVertexData *vd =
			vs->octree.vertices.ptrs[point_location];
		*id = vd->id;
		return vd;
	}

	// Then check in the short list.
	point_location = find_point_in_array(&vs->short_list, &p, vs->epsilon);
	if (point_location < vs->short_list.size) {
		struct GeoVertexData *vd =
			vs->short_list.ptrs[point_location];
		*id = vd->id;
		return vd;
	}

	// We don't have this point yet. Create a vertex and return its id
	// and GeoVertexData pointer.
	*id = vs->next_id;
	++vs->next_id;
	GeoHTInsert(&vs->id_map, *id, LOC_IN_SHORT_TABLE | vs->size);
	if (vs->size == vs->capacity) {
		static const double growth_factor = 1.7;
		vs->capacity *= growth_factor;
		vs->vertex_data = realloc(vs->vertex_data, vs->capacity);
	}
	GeoVDInitialize(&vs->vertex_data[vs->size]);
	vs->vertex_data[vs->size].id = *id;
	vs->vertex_data[vs->size].edge_list = 0;
	struct GeoVertexData *vd = &vs->vertex_data[vs->size];
	++vs->size;
	struct GeoVertex vertex = {p, vd};
	push_back_vertex(&vs->short_list, vertex);

	return vd;
}

