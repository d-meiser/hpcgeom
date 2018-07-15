#include <transformation.h>
#include <geo_config.h>
#include <assert.h>


#define MY_ALIGNMENT 128
#define MY_CHUNK_SIZE 16


#ifdef GEO_HAVE_FUNCTION_MULTI_DISPATH
__attribute__((target_clones("avx","sse4.2","default")))
#endif
static void apply_transform(double T[3][4], double **x, double**y, int n)
{

	int num_chunks = n / MY_CHUNK_SIZE;
	asm("# Chunked part");
	for (int i = 0; i < num_chunks; ++i) {
		double *restrict x0 = __builtin_assume_aligned(x[0] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict x1 = __builtin_assume_aligned(x[1] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict x2 = __builtin_assume_aligned(x[2] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict y0 = __builtin_assume_aligned(y[0] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict y1 = __builtin_assume_aligned(y[1] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict y2 = __builtin_assume_aligned(y[2] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);

		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			y0[j] = T[0][0] * x0[j] + T[0][1] * x1[j] + T[0][2] * x2[j] + T[0][3];
		}
		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			y1[j] = T[1][0] * x0[j] + T[1][1] * x1[j] + T[1][2] * x2[j] + T[1][3];
		}
		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			y2[j] = T[2][0] * x0[j] + T[2][1] * x1[j] + T[2][2] * x2[j] + T[2][3];
		}
	}

	asm("# Cleanup");
	{
		double *restrict x0 = __builtin_assume_aligned(x[0] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict x1 = __builtin_assume_aligned(x[1] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict x2 = __builtin_assume_aligned(x[2] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict y0 = __builtin_assume_aligned(y[0] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict y1 = __builtin_assume_aligned(y[1] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict y2 = __builtin_assume_aligned(y[2] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		for (int i = num_chunks * MY_CHUNK_SIZE; i < n; ++i) {
			y0[i] = T[0][0] * x0[i] + T[0][1] * x1[i] + T[0][2] * x2[i] + T[0][3];
			y1[i] = T[1][0] * x0[i] + T[1][1] * x1[i] + T[1][2] * x2[i] + T[1][3];
			y2[i] = T[2][0] * x0[i] + T[2][1] * x1[i] + T[2][2] * x2[i] + T[2][3];
		}
	}
}

#ifdef GEO_HAVE_FUNCTION_MULTI_DISPATH
__attribute__((target_clones("avx","sse4.2","default")))
#endif
static void apply_transform_inplace(double T[3][4], double **x, int n)
{

	int num_chunks = n / MY_CHUNK_SIZE;
	asm("# Chunked part");
	for (int i = 0; i < num_chunks; ++i) {
		double *restrict x0 = __builtin_assume_aligned(x[0] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict x1 = __builtin_assume_aligned(x[1] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict x2 = __builtin_assume_aligned(x[2] + i * MY_CHUNK_SIZE, MY_ALIGNMENT);

		double tmp[3][MY_CHUNK_SIZE];
		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			tmp[0][j] = T[0][0] * x0[j] + T[0][1] * x1[j] + T[0][2] * x2[j] + T[0][3];
		}
		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			tmp[0][j] = T[1][0] * x0[j] + T[1][1] * x1[j] + T[1][2] * x2[j] + T[1][3];
		}
		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			tmp[0][j] = T[2][0] * x0[j] + T[2][1] * x1[j] + T[2][2] * x2[j] + T[2][3];
		}
		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			x0[j] = tmp[0][j];
		}
		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			x1[j] = tmp[1][j];
		}
		for (int j = 0; j < MY_CHUNK_SIZE; ++j) {
			x2[j] = tmp[2][j];
		}
	}

	asm("# Cleanup");
	{
		double *restrict x0 = __builtin_assume_aligned(x[0] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict x1 = __builtin_assume_aligned(x[1] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		double *restrict x2 = __builtin_assume_aligned(x[2] + num_chunks * MY_CHUNK_SIZE, MY_ALIGNMENT);
		for (int i = num_chunks * MY_CHUNK_SIZE; i < n; ++i) {
			double tmp[3];
			tmp[0] = T[0][0] * x0[i] + T[0][1] * x1[i] + T[0][2] * x2[i] + T[0][3];
			tmp[1] = T[1][0] * x0[i] + T[1][1] * x1[i] + T[1][2] * x2[i] + T[1][3];
			tmp[2] = T[2][0] * x0[i] + T[2][1] * x1[i] + T[2][2] * x2[i] + T[2][3];
			x0[i] = tmp[0];
			x1[i] = tmp[1];
			x2[i] = tmp[2];
		}
	}
}


void GeoApplyTransform(double T[3][4], const struct GeoVertexArray *a,
	struct GeoVertexArray *b)
{
	assert(b->size >= a->size);
	double *x[3] = {a->x, a->y, a->z};
	double *y[3] = {b->x, b->y, b->z};
	apply_transform(T, x, y, a->size);
}

void GeoApplyTransformInplace(double T[3][4], struct GeoVertexArray *a)
{
	double *x[3] = {a->x, a->y, a->z};
	apply_transform_inplace(T, x, a->size);
}
