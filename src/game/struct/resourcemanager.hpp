#pragma once
#include "game/struct/graphics/surface.hpp"
#include <map>
#include <string>

// Namings matched.
class ResourceManager
{
  public:
    struct Resource
    {
        Surface* m_pSurface;
        int m_type;
    };
    virtual ~ResourceManager(){};
    std::map<std::string, ResourceManager::Resource*> m_data;

    Resource* FindDataByKey(const std::string& keyName)
    {
        std::map<std::string, Resource*>::iterator itor = m_data.find(keyName);

        if (itor != m_data.end())
        {
            // found it
            return itor->second;
        }
        return NULL; // fail
    }

    template <class T>
    T* GetSurfaceResource(const std::string& fileName, Surface::eTextureType textureType, bool bAddBasePath,
                          bool b3dResource, bool loadedByGame)
    {
        if (fileName.empty())
            return NULL;

        Resource* pData = FindDataByKey(fileName);
        if (!pData)
        {
            Surface* pSurf = new T;
            pSurf->m_texType = textureType;

            if (!pSurf->LoadFile(fileName, bAddBasePath))
            {
                delete pSurf;

                printf("ResourceManager::GetSurfaceResource: Unable to load %s\n", fileName.c_str());
                return NULL;
            }

            pData = new Resource;
            if (!pData)
            {
                delete pSurf;
                return NULL;
            }

            pData->m_type = 1;
            pData->m_pSurface = pSurf;
            m_data[fileName] = pData;
        }

        if (pData->m_type == 1)
            return dynamic_cast<T*>(pData->m_pSurface);
        return NULL;
    }

    SurfaceAnim* GetSurfaceAnim(const std::string& fileName, bool bAddBasePath)
    {
        return GetSurfaceResource<SurfaceAnim>(fileName, Surface::TYPE_GUI, bAddBasePath, 0, 1);
    }
};