// #include "IDevice.h"
#include <Arduino.h>

class IDevice
{
  public:
    typedef void (*callBack)();
  
    virtual void initialise();
    virtual void connect() = 0;

    virtual void setOnConnectedEvent(callBack ptr_onConnectedEvent) = 0;
    virtual void setOnDisconnectedEvent(callBack ptr_onDisconnectedEvent) = 0;
    virtual void setOnNotifyEvent(callBack ptr_onNotifyEvent) = 0;

  // protected:
    
    callBack _onConnectedEvent;
    callBack _onDisconnectedEvent;
    callBack _onNotifyEvent;
};

class TestIDeviceClass : public IDevice {
  public:
    TestIDeviceClass() {

    }

    void initialise() {
      Serial.printf("Initialised()! \n");
    }

    void connect() {
      Serial.printf("connect()! \n");
      _onConnectedEvent();
    }

    void setOnConnectedEvent(callBack ptr_onConnectedEvent) {
      _onConnectedEvent = ptr_onConnectedEvent;
    }
    void setOnDisconnectedEvent(callBack ptr_onDisconnectedEvent) {

    }
    void setOnNotifyEvent(callBack ptr_onNotifyEvent) {

    }

};