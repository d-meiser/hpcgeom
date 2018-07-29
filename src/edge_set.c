#include <edge_set.h>
#include <string.h>


void GeoESInitialize(struct GeoEdgeSet *es,
	struct GeoVertexSet *vs)
{
	memset(es, 0, sizeof(*es));
	es->vs = vs;
}

void GeoESDestroy(struct GeoEdgeSet *es)
{
	(void)(es);
}
