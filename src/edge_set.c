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

void GeoESInsert(struct GeoEdgeSet *es, struct GeoPoint p1, struct GeoPoint p2,
	GeoEdgeId *id)
{
	(void)es;
	(void)p1;
	(void)p2;
	*id = 0x0ul;
}
