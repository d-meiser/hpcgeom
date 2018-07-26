#include <edge_array.h>
#include <string.h>
#include <stdlib.h>


void GeoEAInitialize(struct GeoEdgeArray *ea, struct GeoVertexSet *vertices)
{
	memset(ea, 0, sizeof(*ea));
	ea->vertices = vertices;
}

void GeoEADestroy(struct GeoEdgeArray *ea)
{
	free(ea->edges);
	memset(ea, 0 , sizeof(*ea));
}


void GeoEAResize(struct GeoEdgeArray *ea, int size)
{
	if (size > ea->capacity) {
		ea->capacity = (size + 0xF) ^ 0xF;
		ea->edges = realloc(ea->edges, ea->capacity);
	}
}
