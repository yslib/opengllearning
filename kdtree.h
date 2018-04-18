#ifndef KDTREE_H
#define KDTREE_H

#include "shape.h"
#include <vector>

class KDTreeAccelerator:public Shape
{
	std::vector<std::shared_ptr<Shape>> m_shapes;

public:
	KDTreeAccelerator(std::vector<std::shared_ptr<Shape>> & shapes):Shape(nullptr) {}

};

#endif // KDTREE_H