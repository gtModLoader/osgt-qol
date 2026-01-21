#pragma once

#include "boost/signal.hpp"
#include "boost/signals/trackable.hpp"

class GamepadProvider : public boost::signals::trackable
{
  public:
    GamepadProvider();
    virtual ~GamepadProvider(){};
    virtual std::string GetName() = 0;
    virtual bool Init() = 0;
    virtual void Kill() = 0;
    virtual void Update() = 0;

    boost::signal<void(void*)> m_sig_failed_to_connect;
    boost::signal<void(void*, void*)> m_sig_status;

  protected:
  private:
};