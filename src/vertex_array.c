#include <vertex_array.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <geo_config.h>


#define ALIGNED_CHUNKED_COPY_CHUNK_SIZE 8
#define DALIGN (GEO_VA_ALIGNMENT / sizeof(double))

static_assert(sizeof(double) == 8, "Size of double must be 8.");
static_assert(sizeof(void*) == 8, "Size of void* must be 8.");
static_assert(GEO_VA_ALIGNMENT % (ALIGNED_CHUNKED_COPY_CHUNK_SIZE * sizeof(double)) == 0,
    "VertexArray::ALIGNMENT must be a multiple of CHUNK_SIZE * sizeof(double)");


static int required_storage(int capacity, int alignment)
{
	return (capacity * sizeof(double) / alignment + 1) * alignment * 4;
}

static void* make_aligned(void* ptr, int alignment)
{
	uint64_t p = (uint64_t)ptr;
	int misalignment = p % alignment;
	if (misalignment) {
		p = p + (alignment - misalignment);
	}
	return (void*)p;
}

static void set_ptrs(void* d, int capacity, int alignment,
		double** x, double** y, double** z, void*** ptrs)
{
	*x = (double*)d;
	*x = make_aligned(*x, alignment);
	*y = *x + capacity;
	*y = make_aligned(*y, alignment);
	*z = *y + capacity;
	*z = make_aligned(*z, alignment);
	*ptrs = (void**)*z + capacity;
	*ptrs = make_aligned(*ptrs, alignment);
}

#ifdef GEO_HAVE_FUNCTION_MULTI_DISPATH
__attribute__((target_clones("avx2","avx","sse4.2","default")))
#endif
static void aligned_chunked_copy_double(double* dest, const double* src, int n)
{
	assert(n % ALIGNED_CHUNKED_COPY_CHUNK_SIZE == 0);
	int num_chunks = n / ALIGNED_CHUNKED_COPY_CHUNK_SIZE;
	double* restrict x =
		__builtin_assume_aligned(dest, GEO_VA_ALIGNMENT);
	const double* restrict y =
		__builtin_assume_aligned(src, GEO_VA_ALIGNMENT);
	for (int i = 0; i < num_chunks; ++i) {
		for (int j = 0; j < ALIGNED_CHUNKED_COPY_CHUNK_SIZE; ++j) {
			x[i * ALIGNED_CHUNKED_COPY_CHUNK_SIZE + j] =
				y[i * ALIGNED_CHUNKED_COPY_CHUNK_SIZE + j];
		}
	}
}

#ifdef GEO_HAVE_FUNCTION_MULTI_DISPATH
__attribute__((target_clones("avx2","avx","sse4.2","default")))
#endif
static void aligned_chunked_copy_uint64_t(uint64_t* dest, const uint64_t* src,
					  int n)
{
	assert(n % ALIGNED_CHUNKED_COPY_CHUNK_SIZE == 0);
	int num_chunks = n / ALIGNED_CHUNKED_COPY_CHUNK_SIZE;
	uint64_t* restrict x =
		__builtin_assume_aligned(dest, GEO_VA_ALIGNMENT);
	const uint64_t* restrict y =
		__builtin_assume_aligned(src, GEO_VA_ALIGNMENT);
	for (int i = 0; i < num_chunks; ++i) {
		for (int j = 0; j < ALIGNED_CHUNKED_COPY_CHUNK_SIZE; ++j) {
			x[i * ALIGNED_CHUNKED_COPY_CHUNK_SIZE + j] =
				y[i * ALIGNED_CHUNKED_COPY_CHUNK_SIZE + j];
		}
	}
}


void GeoVAInitialize(struct GeoVertexArray* va)
{
	memset(va, 0, sizeof(*va));;
	va->capacity = 16;
	va->data = malloc(required_storage(va->capacity, GEO_VA_ALIGNMENT));
	set_ptrs(va->data, va->capacity, GEO_VA_ALIGNMENT,
			&va->x, &va->y, &va->z, &va->ptrs);
}

void GeoVADestroy(struct GeoVertexArray* va)
{
	free(va->data);
	memset(va, 0, sizeof(*va));;
}

void GeoVAResize(struct GeoVertexArray* va, int size)
{
	if (size > va->capacity) {
		int new_capacity = (size / DALIGN + 1) * DALIGN;
		void* new_data = malloc(required_storage(new_capacity,
					GEO_VA_ALIGNMENT));
		double* new_x;
		double* new_y;
		double* new_z;
		void** new_ptrs;
		set_ptrs(new_data, new_capacity, GEO_VA_ALIGNMENT,
				&new_x, &new_y, &new_z, &new_ptrs);
		asm("# Copying old arrays to new arrays");
		aligned_chunked_copy_double(new_x, va->x, va->capacity);
		aligned_chunked_copy_double(new_y, va->y, va->capacity);
		aligned_chunked_copy_double(new_z, va->z, va->capacity);
		aligned_chunked_copy_uint64_t((uint64_t*)new_ptrs,
					      (const uint64_t*)va->ptrs,
					      va->capacity);
		free(va->data);
		va->data = new_data;
		va->x = new_x;
		va->y = new_y;
		va->z = new_z;
		va->ptrs = new_ptrs;
		va->capacity = new_capacity;
	}
	va->size = size;
}

struct GeoVertexArray GeoVACopy(const struct GeoVertexArray* va)
{
	struct GeoVertexArray copy;
	memset(&copy, 0, sizeof(copy));;
	GeoVAResize(&copy, va->size);
	aligned_chunked_copy_double(copy.x, va->x, copy.capacity);
	aligned_chunked_copy_double(copy.y, va->y, copy.capacity);
	aligned_chunked_copy_double(copy.z, va->z, copy.capacity);
	aligned_chunked_copy_uint64_t((uint64_t*)copy.ptrs,
				      (const uint64_t*)va->ptrs, copy.capacity);
	return copy;
}

void GeoVASwap(struct GeoVertexArray *va1, struct GeoVertexArray *va2)
{
	struct GeoVertexArray tmp = *va1;
	*va1 = *va2;
	*va2 = tmp;
}
