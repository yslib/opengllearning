#ifndef CORE_H
#define CORE_H


#ifdef FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <list>
#include <memory>

/*
 * Qt Headers
 */
#include <QDebug>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QColor>

#include <QMatrix4x4>


constexpr Float PI = 3.1415926535;
constexpr Float LOWEST_Float_VALUE = std::numeric_limits<Float>::lowest();
constexpr Float MAX_Float_VALUE = std::numeric_limits<Float>::max();


 /*
  * Vector2D
 */





typedef QMatrix4x4 Trans3DMat;
typedef QVector3D Point3f;
typedef QVector3D Vector3f;
typedef QVector2D Point2f;
typedef QVector2D Vector2f;

typedef QPoint Point2i;
typedef QPoint Vector2i;
typedef Point3f Color;




//Vertex for subdivision
//Face for subdivision






#endif // SHAPE_H
