#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <vertex_array.h>
#include <geo_export.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoVertexArray;

GEO_EXPORT void GeoApplyTransform(double T[3][4],
	const struct GeoVertexArray *a, struct GeoVertexArray *b);
GEO_EXPORT void GeoApplyTransformInplace(double T[3][4],
	struct GeoVertexArray *a);

#ifdef __cplusplus
}
#endif

#endif

