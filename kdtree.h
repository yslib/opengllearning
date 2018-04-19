#ifndef KDTREE_H
#define KDTREE_H

#include "shape.h"
#include <vector>

class KDTreeAccelerator:public Shape
{
	std::vector<std::shared_ptr<Shape>> m_shapes;
    struct KDTreeNode{

        union{
            Float split;
            int onePrimtive;
            int primitiveIndicesOffset;
        };
        union{
            int flag;
            int nPrim;
            int aboveChild;
        };
    };

public:
	KDTreeAccelerator(std::vector<std::shared_ptr<Shape>> & shapes):Shape(nullptr) {}

};

#endif // KDTREE_H
