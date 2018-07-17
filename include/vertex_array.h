#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <geo_export.h>


#ifdef __cplusplus
extern "C" {
#endif


#define GEO_VA_ALIGNMENT 128

struct GeoVertexArray {
	int size;
	int capacity;
	void* data;
	double* x;
	double* y;
	double* z;
	void** ptrs;
};

GEO_EXPORT void GeoVAInitialize(struct GeoVertexArray* va);
GEO_EXPORT void GeoVADestroy(struct GeoVertexArray* va);
GEO_EXPORT void GeoVAResize(struct GeoVertexArray* va, int size);
GEO_EXPORT struct GeoVertexArray GeoVACopy(const struct GeoVertexArray* va);
GEO_EXPORT void GeoVASwap(struct GeoVertexArray *va1,
	struct GeoVertexArray *va2);

#ifdef __cplusplus
}
#endif

#endif
