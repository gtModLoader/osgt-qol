#include "entityutils.hpp"

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
        // TODO: This will not trigger in development builds anymore. replace with a custom assert
        // macro that doesn't rely on NDEBUG, and instead relies on our own OSGT_QOL_DEVELOPMENT
        // macro. As of now, this is the only assertion in the codebase
        assert(!"Huh?");
        return;
    }

    pComp->GetVar("shadowColor")->Set(color);
}