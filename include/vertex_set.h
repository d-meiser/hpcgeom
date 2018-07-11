#ifndef VERTEX_SET_H
#define VERTEX_SET_H

#include <vertex_array.h>
#include <basic_types.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoVertexPtr {
	int ptr;
	int ref_count;
};

struct GeoVertexSet {
	struct GeoBoundingBox bbox;
	struct GeoVertexPtr* vertex_table;
	struct GeoVertexArray vertices;
};

void GeoVSInitialize(struct GeoVertexSet* vs);
void GeoVSFree(struct GeoVertexSet* vs);
void* GeoVSInsert(struct GeoVertex* v, int* id);
void GeoVSRelease(int id);
int GeoVSGetVertexLocation(int id);

#ifdef __cplusplus
}
#endif

#endif

