#include <hash_table.h>

#include <unordered_map>

struct GeoHashTableImpl {
  std::unordered_map<uint32_t, uint32_t> map;
};

void GeoHTInitialize(struct GeoHashTable *ht) {
  ht->impl = new GeoHashTableImpl;
}

void GeoHTDestroy(struct GeoHashTable *ht) {
  delete (GeoHashTableImpl*)ht->impl;
}

void GeoHTInsert(struct GeoHashTable *ht, uint32_t key, uint32_t value) {
  struct GeoHashTableImpl *impl =
      static_cast<struct GeoHashTableImpl *>(ht->impl);
  impl->map[key] = value;
}

int GeoHTLookup(struct GeoHashTable *ht, uint32_t key, uint32_t *value) {
  struct GeoHashTableImpl *impl =
      static_cast<struct GeoHashTableImpl *>(ht->impl);
  auto i = impl->map.find(key);
  if (i != impl->map.end()) {
    *value = i->second;
    return 1;
  }
  return 0;
}

void GeoHTClear(struct GeoHashTable *ht) {
  struct GeoHashTableImpl *impl =
      static_cast<struct GeoHashTableImpl *>(ht->impl);
  impl->map.clear();
}

