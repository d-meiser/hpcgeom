#include <vertex_set.h>

void GeoVSInitialize(struct GeoVertexSet *vs, struct GeoBoundingBox bbox,
	double epsilon)
{
	GeoHOInitialize(&vs->large, bbox);
	GeoHOInitialize(&vs->small, bbox);
	vs->locations = 0;
	vs->epsilon = epsilon;
}

void GeoVSDestroy(struct GeoVertexSet *vs)
{
	GeoHODestroy(&vs->large);
	GeoHODestroy(&vs->small);
}
