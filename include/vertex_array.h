#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

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

void GeoVAInitialize(struct GeoVertexArray* va);
void GeoVADestroy(struct GeoVertexArray* va);
void GeoVAResize(struct GeoVertexArray* va, int size);
struct GeoVertexArray GeoVACopy(const struct GeoVertexArray* va);

#ifdef __cplusplus
}
#endif

#endif
