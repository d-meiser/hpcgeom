#include <vertex_set.h>
#include <stdlib.h>
#include <string.h>


struct GeoIdTable
{
	uint32_t *locations;
	uint32_t next_id;
	uint32_t capacity;
};

void GeoITInitialize(struct GeoIdTable *table)
{
	memset(table, 0, sizeof(*table));
	table->capacity = 32;
	table->locations = malloc(table->capacity * sizeof(*table->locations));
}

void GeoITDestroy(struct GeoIdTable *table)
{
	free(table->locations);
}

void GeoVSInitialize(struct GeoVertexSet *vs, struct GeoBoundingBox bbox,
	double epsilon)
{
	memset(vs, 0, sizeof(*vs));
	GeoHOInitialize(&vs->large, bbox);
	GeoHOInitialize(&vs->small, bbox);
	vs->id_table = malloc(sizeof(*vs->id_table));
	GeoITInitialize(vs->id_table);
	vs->epsilon = epsilon;
}

void GeoVSDestroy(struct GeoVertexSet *vs)
{
	GeoHODestroy(&vs->large);
	GeoHODestroy(&vs->small);
	GeoITDestroy(vs->id_table);
	free(vs->id_table);
}

void *GeoVSInsert(struct GeoVertex v, int *d)
{
	(void)v;
	*d = 0;
	return (void*)0x2;
}
