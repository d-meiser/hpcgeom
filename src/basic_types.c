#include <basic_types.h>
#include <string.h>
#include <stdlib.h>


void GeoVDInitialize(struct GeoVertexData *vd)
{
	memset(vd, 0, sizeof(*vd));
}

void GeoVDDestroy(struct GeoVertexData *vd)
{
	struct GeoEdgeList *l = vd->edge_list;
	while (l) {
		struct GeoEdgeList *next = l->next;
		free(l);
		l = next;
	}
}

