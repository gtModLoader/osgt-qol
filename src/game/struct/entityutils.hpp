#include "entity.hpp"

// From Proton SDK
void SetTextShadowColor(Entity* pEnt, uint32_t color)
{
    EntityComponent* pComp = pEnt->GetComponentByName("TextRender");

    if (!pComp && ((pComp = pEnt->GetComponentByName("TextBoxRender")) != 0))
    {
    }

    if (!pComp && ((pComp = pEnt->GetComponentByName("LogDisplay")) != 0))
    {
    }

    if (!pComp && ((pComp = pEnt->GetComponentByName("InputTextRender")) != 0))
    {
    }

    if (!pComp)
    {
        assert(!"Huh?");
        return;
    }

    pComp->GetVar("shadowColor")->Set(color);
}