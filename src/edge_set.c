#include <edge_set.h>
#include <string.h>
#include <stdlib.h>


void GeoESInitialize(struct GeoEdgeSet *es)
{
	memset(es, 0, sizeof(*es));
	es->capacity = 16;
	es->edges = malloc(es->capacity * sizeof(*es->edges));
	GeoHTInitialize(&es->id_map);
}

void GeoESDestroy(struct GeoEdgeSet *es)
{
	free(es->edges);
	GeoHTDestroy(&es->id_map);
}

GeoEdgeId GeoESInsert(struct GeoEdgeSet *es, struct GeoEdge edge)
{
	(void)es;
	(void)edge;
	return 0x0u;
}
