#include "core.h"

 //Memory Allocation Functions
void *AllocAligned(size_t size) {
//#if defined(PBRT_HAVE__ALIGNED_MALLOC)
	return _aligned_malloc(size, 64);
//#elif defined(PBRT_HAVE_POSIX_MEMALIGN)
//	void *ptr;
//	if (posix_memalign(&ptr, PBRT_L1_CACHE_LINE_SIZE, size) != 0) ptr = nullptr;
//	return ptr;
//#else
//	return memalign(PBRT_L1_CACHE_LINE_SIZE, size);
//#endif
}

void FreeAligned(void *ptr) {
	if (!ptr) return;
//#if defined(PBRT_HAVE__ALIGNED_MALLOC)
	_aligned_free(ptr);
//#else
//	free(ptr);
//#endif
}


Point3Df weightOneRing(SDVertex * vert, Float beta)
{
	std::vector<Point3Df> verticesOnTheRing(vert->valence());
	vert->oneRing(verticesOnTheRing.data());
	Point3Df p = (1 - vert->valence()*beta)*vert->point();

	for (int i = 0; i<verticesOnTheRing.size(); i++)
	{
		p += beta * verticesOnTheRing[i];
	}
	return p;
}

Point3Df weightBoundary(SDVertex * vert, Float beta)
{
	std::vector<Point3Df> verticesOnTheRing(vert->valence());
	vert->oneRing(verticesOnTheRing.data());
	Point3Df p = (1 - 2 * beta)*vert->point();
	p += beta * verticesOnTheRing.front();
	p += beta * verticesOnTheRing.back();
	return p;
}
