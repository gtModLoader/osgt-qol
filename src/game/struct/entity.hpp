#pragma once
#include "component.hpp"

class Entity
{
  public:
    // Don't include ctor or anything yet. We don't have boost signals.

    void SetName(std::string name);
    std::string GetName() { return m_name; }

    Entity* GetEntityByName(std::string key)
    {
        for (const auto& ent : m_children)
            if (ent->m_name == key)
                return ent;
        return nullptr;
    }
    void GetEntitiesByName(std::vector<Entity*>* pEnts, std::string name)
    {
        if (m_bTaggedForDeletion)
            return;
        if (name == m_name)
            pEnts->push_back(this);

        for (const auto& ent : m_children)
        {
            ent->GetEntitiesByName(pEnts, name);
        }
    }
    void GetEntitiesByName(std::vector<Entity*>* pEnts, std::string name, uint32_t depth)
    {
        if (m_bTaggedForDeletion)
            return;
        if (name == m_name)
            pEnts->push_back(this);
        if (depth == 0)
            return;

        for (const auto& ent : m_children)
        {
            ent->GetEntitiesByName(pEnts, name, depth - 1);
        }
    }
    EntityComponent* GetComponentByName(std::string key)
    {
        for (const auto& comp : m_components)
            if (comp->GetName() == key)
                return comp;
        return nullptr;
    }

    VariantDB* GetShared() { return &m_sharedDB; }
    Variant* GetVar(const std::string& varName) { return m_sharedDB.GetVar(varName); }
    Variant* GetVarWithDefault(const std::string& varName, const Variant& var)
    {
        return m_sharedDB.GetVarWithDefault(varName, var);
    }

    Entity* GetParent() { return m_pParent; }
    void SetParent(Entity* pEntity) { m_pParent = pEntity; }

    // We do not include Function methods right now.

    std::list<Entity*>* GetChildren() { return &m_children; }
    std::list<EntityComponent*>* GetComponents() { return &m_components; }

    void PrintTreeAsText(int indent = 0)
    {
        std::string us;

        for (int i = 0; i < indent; i++)
        {
            us += "  "; // make the tree have branches more visually
        }
        us += m_name;

        if (!m_components.empty())
        {
            us += " (";
            for (auto it = m_components.begin(); it != m_components.end(); it++)
            {
                if (it != m_components.begin())
                    us += ", ";
                us += (*it)->GetName();
            }

            us += ")";
        }

        printf("%s\n", us.c_str());
        m_sharedDB.Print();

        for (auto& ent : m_children)
        {
            ent->PrintTreeAsText(indent + 1);
        }
    }

  private:
    uint8_t pad[72];                          // 0
    std::string m_name;                       // 72
    std::list<Entity*> m_children;            // 104
    std::list<EntityComponent*> m_components; // 120
    VariantDB m_sharedDB;                     // 136
    Entity* m_pParent;                        // 280
    bool m_bTaggedForDeletion;                // 288
    int m_recursiveFilterReferences;          // 292
    Variant* m_pPosVarCache;                  // 296
    CL_Vec2f* m_pSizeCache;                   // 304
    uint32_t* m_pAlignment;                   // 312
};