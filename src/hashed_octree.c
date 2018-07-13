#include <hashed_octree.h>
#include <string.h>
#include <stdlib.h>


void GeoHOInitialize(struct GeoHashedOctree* tree, struct GeoBoundingBox b)
{
	memset(tree, 0, sizeof(*tree));
	GeoVAInitialize(&tree->vertices);
	tree->hashes = malloc(tree->vertices.capacity * sizeof(*tree->hashes));
	tree->bbox = b;
}

void GeoHODestroy(struct GeoHashedOctree* tree)
{
	GeoVADestroy(&tree->vertices);
	free(tree->hashes);
}

void GeoHOInsert(struct GeoHashedOctree *tree,
	const struct GeoVertexArray *va, int begin, int end)
{
	(void)tree;
	(void)va;
	(void)begin;
	(void)end;
}
