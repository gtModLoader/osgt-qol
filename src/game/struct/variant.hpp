#pragma once
#include "vec.hpp"
#include <string>
#include <windows.h>

#include <boost/signal.hpp>

class Entity;
class EntityComponent;
class Variant
{
  public:
    enum eType
    {
        TYPE_UNUSED,
        TYPE_FLOAT,
        TYPE_STRING,
        TYPE_VECTOR2,
        TYPE_VECTOR3,
        TYPE_UINT32,
        TYPE_ENTITY,
        TYPE_COMPONENT,
        TYPE_RECT,
        TYPE_INT32

    };
    Variant(uint32_t var)
    {
        SetDefaults();
        Set(var);
    }
    Variant(int32_t var)
    {
        SetDefaults();
        Set(var);
    }

    Variant(float var)
    {
        SetDefaults();
        Set(var);
    }
    Variant(const std::string& var)
    {
        SetDefaults();
        Set(var);
    }
    Variant(float x, float y)
    {
        SetDefaults();
        Set(CL_Vec2f(x, y));
    }
    Variant(float x, float y, float z)
    {
        SetDefaults();
        Set(CL_Vec3f(x, y, z));
    }
    Variant(const CL_Vec2f& v2)
    {
        SetDefaults();
        Set(v2);
    }
    Variant(const CL_Vec3f& v3)
    {
        SetDefaults();
        Set(v3);
    }
    Variant(const CL_Rectf& r)
    {
        SetDefaults();
        Set(r);
    }
    Variant(Entity* pEnt)
    {
        SetDefaults();
        Set(pEnt);
    }
    Variant(EntityComponent* pEnt)
    {
        SetDefaults();
        Set(pEnt);
    }

    Variant() { SetDefaults(); }
    ~Variant()
    {
        // SAFE_DELETE(m_pSig_onChanged);
        if (m_pSig_onChanged)
        {
            delete m_pSig_onChanged;
            m_pSig_onChanged = 0;
        }
    };

    void Reset()
    {
        m_type = TYPE_UNUSED;
        // SAFE_DELETE(m_pSig_onChanged);
        if (m_pSig_onChanged)
        {
            delete m_pSig_onChanged;
            m_pSig_onChanged = 0;
        }
    }

    boost::signal<void(Variant*)>* GetSigOnChanged()
    {
        if (!m_pSig_onChanged)
        {
            m_pSig_onChanged = new boost::signal<void(Variant*)>;
        }
        return m_pSig_onChanged;
    }

    void Set(const Variant& v);
    void SetVariant(Variant* pVar);

    void Set(float var)
    {
        m_type = TYPE_FLOAT;
        *((float*)m_var) = var;
        if (m_pSig_onChanged)
            (*m_pSig_onChanged)(this);
    }

    float& GetFloat()
    {
        if (m_type == TYPE_UNUSED)
            Set(float(0));
        return *((float*)m_var);
    }

    const float& GetFloat() const { return *((float*)m_var); }

    void Set(const Entity* pEnt)
    {
        m_type = TYPE_ENTITY;
        m_pVoid = (void*)pEnt;
    }

    Entity* GetEntity()
    {
        if (m_type == TYPE_UNUSED)
            Set((Entity*)NULL);
        return ((Entity*)m_pVoid);
    }

    const Entity* GetEntity() const { return ((Entity*)m_pVoid); }

    void Set(const EntityComponent* pEnt)
    {
        m_type = TYPE_COMPONENT;
        m_pVoid = (void*)pEnt;
    }

    EntityComponent* GetComponent()
    {
        if (m_type == TYPE_UNUSED)
            Set((Entity*)NULL);
        return ((EntityComponent*)m_pVoid);
    }

    const EntityComponent* GetComponent() const { return ((EntityComponent*)m_pVoid); }

    void Set(uint32_t var)
    {
        m_type = TYPE_UINT32;
        *((uint32_t*)m_var) = var;
    }

    uint32_t& GetUINT32()
    {
        if (m_type == TYPE_UNUSED)
            Set(uint32_t(0));
        return *((uint32_t*)m_var);
    }

    const uint32_t& GetUINT32() const { return *((uint32_t*)m_var); }

    void Set(int32_t var)
    {
        m_type = TYPE_INT32;
        *((int32_t*)m_var) = var;
    }

    int32_t& GetINT32()
    {
        if (m_type == TYPE_UNUSED)
            Set(int32_t(0));
        return *((int32_t*)m_var);
    }

    const int32_t& GetINT32() const { return *((int32_t*)m_var); }

    void Set(std::string const& var)
    {
        m_type = TYPE_STRING;
        m_string = var;
    }

    std::string& GetString() { return m_string; }
    const std::string& GetString() const { return m_string; }

    void Set(CL_Vec2f const& var)
    {
        m_type = TYPE_VECTOR2;
        *((CL_Vec2f*)m_var) = var;
    }
    CL_Vec2f& GetVector2()
    {
        if (m_type == TYPE_UNUSED)
            Set(CL_Vec2f(0, 0));
        return *((CL_Vec2f*)m_var);
    }
    const CL_Vec2f& GetVector2() const { return *((CL_Vec2f*)m_var); }

    void Set(float x, float y) { Set(CL_Vec2f(x, y)); }

    void Set(CL_Vec3f const& var)
    {
        m_type = TYPE_VECTOR3;
        *((CL_Vec3f*)m_var) = var;
    }
    CL_Vec3f& GetVector3()
    {
        if (m_type == TYPE_UNUSED)
            Set(CL_Vec3f(0, 0, 0));
        return *((CL_Vec3f*)m_var);
    }
    const CL_Vec3f& GetVector3() const { return *((CL_Vec3f*)m_var); }

    void Set(CL_Rectf const& var)
    {
        m_type = TYPE_RECT;
        *((CL_Rectf*)m_var) = var;
    }
    CL_Rectf& GetRect()
    {
        if (m_type == TYPE_UNUSED)
            Set(CL_Rectf(0, 0, 0, 0));
        return *((CL_Rectf*)m_var);
    }
    const CL_Rectf& GetRect() const { return *((CL_Rectf*)m_var); }

    void Set(float x, float y, float z) { Set(CL_Vec3f(x, y, z)); }

    eType GetType() const { return m_type; }

    std::string Print()
    {
        switch (m_type)
        {
        case TYPE_FLOAT:
            return std::to_string(*((float*)m_var));
            break;
        case TYPE_STRING:
            return m_string;
            break;
        case TYPE_VECTOR2:
        {
            CL_Vec2f* v = reinterpret_cast<CL_Vec2f*>(m_var);
            return v->Print();
            break;
        }
        case TYPE_VECTOR3:
        {
            CL_Vec3f* v = reinterpret_cast<CL_Vec3f*>(m_var);
            return v->Print();
            break;
        }
        case TYPE_UINT32:
            return std::to_string(*(int32_t*)m_var);
            break;
        case TYPE_INT32:
            return std::to_string(*(int32_t*)m_var);
            break;
        case TYPE_ENTITY:
            return "An entity";
            break;
        case TYPE_COMPONENT:
            return "A component";
            break;
        case TYPE_RECT:
        {
            CL_Rectf* v = reinterpret_cast<CL_Rectf*>(m_var);
            return v->Print();
            break;
        }
        case TYPE_UNUSED:
            return "Unknown";
            break;
        }

        return "Unknown";
    }

    void SetDefaults()
    {
        m_type = TYPE_UNUSED;
        m_pSig_onChanged = NULL;
    }
    eType m_type;
    void* m_pVoid;
    union
    {
        uint8_t m_var[16];
        float m_as_floats[4];
        uint32_t m_as_uint32s[4];
        int32_t m_as_int32s[4];
    };
    std::string m_string;
    boost::signal<void(Variant*)>* m_pSig_onChanged;
};

class VariantList
{
  public:
    VariantList() {};

    Variant& Get(int parmNum) { return m_variant[parmNum]; }
    VariantList(Variant v0) { m_variant[0] = v0; }
    VariantList(Variant v0, Variant v1)
    {
        m_variant[0] = v0;
        m_variant[1] = v1;
    }
    VariantList(Variant v0, Variant v1, Variant v2)
    {
        m_variant[0] = v0;
        m_variant[1] = v1;
        m_variant[2] = v2;
    }
    VariantList(Variant v0, Variant v1, Variant v2, Variant v3)
    {
        m_variant[0] = v0;
        m_variant[1] = v1;
        m_variant[2] = v2;
        m_variant[3] = v3;
    }
    VariantList(Variant v0, Variant v1, Variant v2, Variant v3, Variant v4)
    {
        m_variant[0] = v0;
        m_variant[1] = v1;
        m_variant[2] = v2;
        m_variant[3] = v3;
        m_variant[4] = v4;
    }
    VariantList(Variant v0, Variant v1, Variant v2, Variant v3, Variant v4, Variant v5)
    {
        m_variant[0] = v0;
        m_variant[1] = v1;
        m_variant[2] = v2;
        m_variant[3] = v3;
        m_variant[4] = v4;
        m_variant[5] = v5;
    }

    void Reset()
    {
        for (int i = 0; i < 6; i++)
        {
            m_variant[i].Reset();
        }
    }

    Variant m_variant[6];
};