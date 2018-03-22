#ifndef _OBJREADER_H_
#define _OBJREADER_H_
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>
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
    ObjReader(const std::string  & fileName,const std::string & mtlFileName = std::string())noexcept;
    bool load(const std::string & fileName,const std::string & mtlFileName = std::string());
    bool isLoaded()const;

    std::size_t getVertexCount()const { return m_vertices.size(); }
    std::size_t getNormalCount()const { return m_normals.size(); }
    std::size_t getTextureCoordCount()const { return m_textures.size(); }
    std::size_t getFacesCount()const { return m_vertexIndices.size(); }

    const std::vector<Point3f> & getVertices()const { return m_vertices; }
    const std::vector<int> & getFaceIndices()const { return m_vertexIndices; }
    const std::vector<Vector3f> & getNormals()const { return m_normals; }

    const Point3f * getVerticesFlatArray()const { return m_vertices.data(); }
    const Vector3f * getNormalsFlatArray()const { return m_normals.data(); }
    const int * getFacesIndicesFlatArray()const { return m_vertexIndices.data(); }

    virtual ~ObjReader();
private:
    void _init();
private:
    bool m_loaded;
    std::vector<Vector3f> m_normals;
    std::vector<Point3f> m_vertices;
    std::vector<Point2f> m_textures;
    std::unordered_map<int,std::string> m_indexToMtlName;
    std::vector<int> m_vertexIndices;
    std::vector<int> m_normalIndices;
    std::vector<int> m_textureIndices;

};

class MaterialReader
{
public:
    typedef std::unordered_map<std::string, Color> PropertyMap;
    typedef std::unordered_map<std::string, PropertyMap> MaterialLib;
private:
    bool m_loaded;
    MaterialLib m_mLib;
    int test;
public:
    MaterialReader(const std::string & fileName)
    {
        m_loaded = loadFromFile(fileName);
    }
    bool loadFromFile(const std::string & fileName)
    {
        std::ifstream fileIn(fileName);
        if (fileIn.is_open() == false)
            return m_loaded = false;
        std::string line;
        std::string prevMtlName;
        while(getline(fileIn,line))
        {
            line = line.substr(0, line.find_first_of('#'));
            std::stringstream ss(line);
            std::string keyword;
            ss >> keyword;
            if(keyword == "newmtl")     //a new material defination
            {
                std::string mtlName;
                ss >> mtlName;
                prevMtlName = mtlName;
            }
            else
            {
                float value;
                int i = 0;
                while(ss>>value)
                {
                    m_mLib[prevMtlName][keyword][i] = value;
                    i++;
                }
            }
        }
    }
    PropertyMap & operator[](const std::string & name)
    {
        return m_mLib[name];
    }
    bool isLoaded()const
    {
        return m_loaded;
    }
};
#endif
