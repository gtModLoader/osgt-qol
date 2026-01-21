#pragma once

#include "gamepad.hpp"
#include "gamepadprovider.hpp"
#include <list>
#include <vector>

class GamepadManager
{
  public:
    GamepadManager();
    virtual ~GamepadManager(){};

    void ClearProviders()
    {
        std::list<GamepadProvider*>::iterator itor = this->m_providers.begin();
        for (; itor != this->m_providers.end(); itor++)
        {
            (*itor)->Kill();
            delete *itor;
        }
        this->m_providers.clear();

        for (uint32_t i = 0; i < this->m_gamepads.size(); i++)
        {
            // Basically SAFE_DELETE macro
            if (this->m_gamepads[i]) {
                delete (this->m_gamepads[i]);
                this->m_gamepads[i] = nullptr;
            }
        }

        this->m_gamepads.clear();
        this->m_defaultGamepadID = 0;
    }

    void ClearGamepads()
    {
        for (uint32_t i = 0; i < this->m_gamepads.size(); i++)
        {
            if (this->m_gamepads[i]) {
                delete (this->m_gamepads[i]);
                this->m_gamepads[i] = nullptr;
            }
        }

        this->m_gamepads.clear();
        this->m_defaultGamepadID = 0;
    }

    void RemoveGamepadsByProvider(GamepadProvider* provider);

    int m_defaultGamepadID;

    std::list<GamepadProvider*> m_providers;
    std::vector<Gamepad*> m_gamepads;

  private:
};
static_assert(sizeof(GamepadManager) == 56, "GamepadManager class size mismatch.");