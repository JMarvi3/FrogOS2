#include <stdlib.h>

void *heap = (void *)0x300000; // 3MB
size_t heap_size = 0x100000; // 1MB

void *malloc(size_t size) {
  if (size == 0) {
    return 0;
  }
  if (size % 8 != 0) {  // align to 8 bytes
    size += 8 - (size % 8);
  }
  if (heap_size < size) {
    return 0;
  }
  void *ret = heap;
  heap += size;
  heap_size -= size;
  return ret;
}