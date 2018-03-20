#ifndef _OBJREADER_H_
#define _OBJREADER_H_
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include "core.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>



//  [12/23/2017 ysl]
//---------------------------------
//Only polygon is supportted in the ObjReader class
// Single Line
// v(3): vertex
// vt(2): texture coordinate
// vn(3): vertex normal
// p(1): point
// l(2): line
// f():

template<typename T>
using Point3d = std::tuple<T, T, T>;

template<typename T>
using Point2d = std::tuple<T, T>;

template<typename T>
using Vector3d = std::tuple<T, T, T>;


class ObjReader
{
public:
    ObjReader()noexcept;
    ObjReader(const std::string  & fileName)noexcept;
    bool load(const std::string & fileName);
    bool isLoaded()const;

    std::size_t getVertexCount()const { return m_vertices.size(); }
    std::size_t getNormalCount()const { return m_normals.size(); }
    std::size_t getTextureCoordCount()const { return m_textures.size(); }
    std::size_t getFacesCount()const { return m_facesIndices.size(); }

    const std::vector<Point3f> & getVertices()const { return m_vertices; }
    const std::vector<int> & getFaceIndices()const { return m_facesIndices; }
    const std::vector<Vector3f> & getNormals()const { return m_normals; }
    //std::size_t getNormalsCount()const { return m_normals.size(); }

    const Point3f * getVerticesFlatArray()const { return m_vertices.data(); }
    const Vector3f * getNormalsFlatArray()const { return m_normals.data(); }
    //float * getNormalsSFlatArray()const { return m_normalsFlatArray; }
    //float * getTexturesFlayArray()const { return m_texturesFlatArray; }
    const int * getFacesIndicesFlatArray()const { return m_facesIndices.data(); }
    //int * getNormalsIndicesFlayArray()const { return m_normalsIndicesFlatArray; }
    //int * getTextureIndicesFlayArray()const { return m_textureIndicesFlatArray; }
    virtual ~ObjReader();
private:
    void _init();
private:
    //void _toVerticesFlatArray();
    //void _toNormalsSFlatArray();
    //void _toTexturesFlayArray();
    //void _toFacesIndicesFlatArray();
    //void _toFormalsIndicesFlayArray();
    //void _toTextureIndicesFlayArray();

    bool m_loaded;

    std::vector<Vector3f> m_normals;
    std::vector<Point3f> m_vertices;
    std::vector<Point2f> m_textures;
    std::vector<int> m_facesIndices;
    //std::vector<std::vector<int>> m_normalsIndices;
        
    //std::vector<std::vector<int>> m_texturesIndices;
    //float * m_verticesFlatArray;
    //float * m_normalsFlatArray;
    //float * m_texturesFlatArray;
    //int * m_facesIndicesFlatArray;
    //int * m_normalsIndicesFlatArray;
    //int * m_textureIndicesFlatArray;

    //std::unique_ptr<Point3f[]> m_verticesFlatArray;
    //std::unique_ptr<Point3f[]> m_normalsFlatArray;
    //std::unique_ptr<Point2f[]> m_texturesFlatArray;
    //std::unique_ptr<int[]> m_facesIndicesFlatArray;
    //std::unique_ptr<int[]> m_normalsIndicesFlatArray;
    //std::unique_ptr<int[]> m_textureIndicesFlatArray;
};
#endif
