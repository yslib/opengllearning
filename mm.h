#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H
#include <cstddef>

void * alignedAlloc(size_t size,size_t alignement);
void alignedFree(void * aligned);

#endif // MEMORYPOOL_H
