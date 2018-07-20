#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <geo_export.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

struct GeoHashTable
{
  void *impl;
};

GEO_EXPORT void GeoHTInitialize(struct GeoHashTable *ht);
GEO_EXPORT void GeoHTDestroy(struct GeoHashTable *ht);
GEO_EXPORT void GeoHTInsert(struct GeoHashTable *ht, uint32_t key,
	uint32_t value);
GEO_EXPORT int GeoHTLookup(struct GeoHashTable *ht, uint32_t key,
	uint32_t *value);
GEO_EXPORT void GeoHTClear(struct GeoHashTable *ht);

#ifdef __cplusplus
}
#endif

#endif

