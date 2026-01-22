#pragma once
#include "boost/bind/bind.hpp"
#include "component.hpp"

class Entity : public boost::signals::trackable
{
  public:
    Entity() { OneTimeInit(); }
    Entity(std::string name)
    {
        m_name = name;
        OneTimeInit();
    }
    virtual ~Entity()
    {
        sig_onRemoved(this);

        RemoveAllEntities();
        RemoveAllComponents();
    }

    void SetName(std::string name) { m_name = name; }
    std::string GetName() { return m_name; }

    Entity* AddEntity(Entity* pEntity)
    {
        pEntity->SetParent(this);
        m_children.push_back(pEntity);
        return pEntity;
    }

    Entity* AddEntityToFront(Entity* pEntity)
    {
        pEntity->SetParent(this);
        m_children.push_front(pEntity);
        return pEntity;
    }

    void OneTimeInit()
    {
        m_pPosVarCache = NULL;
        m_pAlignment = NULL;
        m_pSizeCache = NULL;
        m_recursiveFilterReferences = 0;
        m_bTaggedForDeletion = false;
        m_pParent = NULL;

        GetFunction("OnDelete")->sig_function.connect(1, boost::bind(&Entity::OnDelete, this, _1));
    }

    void OnDelete(VariantList* pVList)
    {
        if (GetParent())
        {
            GetParent()->RemoveEntityByAddress(this);
        }
        else
        {
            // no parent?  Fine, kill us anyway
            delete this;
            return;
        }
    }

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
    void MoveEntityToTopByAddress(Entity* pEnt)
    {
        if (!RemoveEntityByAddress(pEnt, false))
        {
            printf("Unable to find entity to remove\n");
            return;
        }

        m_children.push_back(pEnt);
    }
    void MoveEntityToBottomByAddress(Entity* pEnt)
    {
        if (!RemoveEntityByAddress(pEnt, false))
        {
            printf("Unable to find entity to remove");
            return;
        }

        m_children.push_front(pEnt);
    }
    bool RemoveEntityByAddress(Entity* pEntToDelete, bool bDeleteAlso = true)
    {
        std::list<Entity*>::iterator itor = m_children.begin();

        while (itor != m_children.end())
        {
            if ((*itor) == pEntToDelete)
            {
                Entity* pTemp = (*itor);
                itor = m_children.erase(itor);
                if (bDeleteAlso)
                {
                    delete (pTemp);
                }
                return true;
            }
            itor++;
        }

        return false;
    }

    EntityComponent* AddComponent(EntityComponent* pComp)
    {
        m_components.push_back(pComp);
        pComp->OnAdd(this);
        return pComp;
    }
    EntityComponent* GetComponentByName(std::string key)
    {
        for (const auto& comp : m_components)
            if (comp->GetName() == key)
                return comp;
        return nullptr;
    }

    bool RemoveComponentByName(const std::string& name)
    {
        std::list<EntityComponent*>::iterator itor = m_components.begin();

        while (itor != m_components.end())
        {
            if ((*itor)->GetName() == name)
            {
                (*itor)->OnRemove();
                delete (*itor);
                itor = m_components.erase(itor);
                return true;
            }
            itor++;
        }

        return false;
    }

    bool RemoveComponentByAddress(EntityComponent* pCompToDelete, bool bDeleteAlso = true)
    {
        std::list<EntityComponent*>::iterator itor = m_components.begin();

        while (itor != m_components.end())
        {
            if ((*itor) == pCompToDelete)
            {
                EntityComponent* pTemp = (*itor);
                itor = m_components.erase(itor);
                if (bDeleteAlso)
                {
                    pTemp->OnRemove();
                    delete (pTemp);
                }
                return true;
            }
            itor++;
        }

        return false;
    }

    VariantDB* GetShared() { return &m_sharedDB; }
    Variant* GetVar(const std::string& varName) { return m_sharedDB.GetVar(varName); }
    Variant* GetVarWithDefault(const std::string& varName, const Variant& var)
    {
        return m_sharedDB.GetVarWithDefault(varName, var);
    }
    FunctionObject* GetFunction(const std::string& funcName)
    {
        return m_sharedDB.GetFunction(funcName);
    }

    Entity* GetParent() { return m_pParent; }
    void SetParent(Entity* pEntity) { m_pParent = pEntity; }

    // We do not include Function methods right now.

    std::list<Entity*>* GetChildren() { return &m_children; }
    std::list<EntityComponent*>* GetComponents() { return &m_components; }

    void RemoveAllEntities()
    {
        std::list<Entity*>::iterator itor = m_children.begin();
        for (; itor != m_children.end();)
        {

            // done this way so entities that want to do searches through entity trees because some
            // OnDelete sig was run won't crash

            Entity* pTemp = (*itor);
            itor = m_children.erase(itor);
            delete pTemp;
        }

        m_children.clear();
    }

    void RemoveAllComponents()
    {
        std::list<EntityComponent*>::iterator itor = m_components.begin();
        for (; itor != m_components.end(); itor++)
        {
            (*itor)->OnRemove();
            delete (*itor);
        }

        m_components.clear();
    }

    void PrintTreeAsText(int indent = 0)
    {
#ifdef OSGT_QOL_DEVELOPMENT
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

        for (auto& ent : m_children)
        {
            ent->PrintTreeAsText(indent + 1);
        }
#endif
    }

    boost::signal<void(Entity*)> sig_onRemoved;

  private:
    std::string m_name;
    std::list<Entity*> m_children;
    std::list<EntityComponent*> m_components;
    VariantDB m_sharedDB;
    Entity* m_pParent;
    bool m_bTaggedForDeletion;
    int m_recursiveFilterReferences;
    Variant* m_pPosVarCache;
    CL_Vec2f* m_pSizeCache;
    uint32_t* m_pAlignment;
};