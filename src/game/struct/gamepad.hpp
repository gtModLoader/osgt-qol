#pragma once

#include "boost/signal.hpp"
#include "boost/signals/trackable.hpp"
#include "game/struct/vec.hpp"

class VariantList;

enum eGamepadButtons
{
    GP_DPAD_BUTTON_LEFT,  // X on xbox controller
    GP_DPAD_BUTTON_UP,    // Y on xbox controller
    GP_DPAD_BUTTON_RIGHT, // B on xbox controller
    GP_DPAD_BUTTON_DOWN,  // A on xbox controller
    GP_DPAD_SELECT,
    GP_DPAD_START,
    GP_DPAD_LBUTTON,
    GP_DPAD_RBUTTON,
    GP_DPAD_LTRIGGER,
    GP_DPAD_RTRIGGER,
    GP_DPAD_HAT_UP, // a hat is like the DPAD thingie on a 360 controller
    GP_DPAD_HAT_RIGHT,
    GP_DPAD_HAT_DOWN,
    GP_DPAD_HAT_LEFT,

    GP_JOYSTICK_BUTTON_LEFT, // you know, like how you can push a joystick "in" and it clicks?
    GP_JOYSTICK_BUTTON_RIGHT,

};
#define GAMEPAD_ID_NONE 0
#define eGamepadID long

#define GAMEPAD_MAX_AXIS 32
#define GAMEPAD_MAX_BUTTONS 16
class GamepadProvider;

class ArcadeInputComponent;

class GamepadButton
{
  public:
    GamepadButton() { m_bDown = false; }
    void OnPress(bool bDown);
    int m_virtualKey;
    bool m_bDown;
};

class GamepadAxis
{
  public:
    GamepadAxis()
    {
        m_useAsButton = 0;
        m_axis = 0;
    }

    float m_axis;
    int m_useAsButton;
};
class MoveButtonState
{
  public:
    MoveButtonState()
    {
        m_bIsDown = false;
        m_releaseTimer = 0;
    }
    bool m_bIsDown;
    unsigned int m_releaseTimer;
    int m_keyType;
};
class Gamepad : public boost::signals::trackable
{
  public:
    Gamepad();
    virtual ~Gamepad(){};
    virtual const std::string& GetName() { return m_name; }
    virtual bool Init() = 0;
    virtual void Kill() = 0;
    virtual void Update();

    boost::signal<void(VariantList*)> m_sig_gamepad_buttons;

    boost::signal<void(VariantList*)> m_sig_left_stick;
    boost::signal<void(VariantList*)> m_sig_right_stick;

    void SetID(eGamepadID id) { m_id = id; }
    eGamepadID GetID() { return m_id; }

    GamepadProvider* GetProvider() { return m_pPadProvider; }
    void SetProvider(GamepadProvider* pProvider) { m_pPadProvider = pProvider; }

    void SendArcadeDirectionByDegrees(int val);
    void OnArcadeCompDestroyed(void* pVList);
    void SendArcadeDirectionRelease();

    bool m_bJustSentStickRelease;
    std::string m_name;
    GamepadProvider* m_pPadProvider;

    GamepadAxis m_axis[GAMEPAD_MAX_AXIS];
    GamepadButton m_buttons[GAMEPAD_MAX_BUTTONS];

    int m_buttonsUsedCount;
    int m_axisUsedCount;
    int m_rightStickAxisX;
    int m_rightStickAxisY;

    CL_Vec2f m_vLastSentLeftStick;
    CL_Vec2f m_vLastSentRightStick;

    int m_id;

    bool m_bIsUsed;

    MoveButtonState m_dirButtons[4];
    ArcadeInputComponent* m_pArcadeComp;
    bool m_bSendLeftStickAsDirectionsToo = false;
    float m_stickAsDirectionDeadZone = 0.15f;
    boost::signals::connection arcade_comp_conn;

  protected:
  private:
};