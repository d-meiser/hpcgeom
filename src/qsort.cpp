#include <qsort.h>

#include <algorithm>

void GeoQsort(uint64_t *x, int n) {
  std::sort(x, x + n);
}

