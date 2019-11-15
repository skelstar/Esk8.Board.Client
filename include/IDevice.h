#include <Arduino.h>

class IDevice
{
  typedef void (*callBack)();

public:
  virtual void initialise();
  virtual void connect() = 0;
  virtual void onConnectedEvent(callBack ptr_onConnectedEvent);
  virtual void onDisconnectedEvent(callBack ptr_onDisconnectedEvent);
  virtual void fireEvents();

protected:
  callBack _onConnectedEvent;
  callBack _onDisconnectedEvent;
};

class BLEDevice1 : public IDevice
{
  typedef void (*callBack)();

public:
  virtual void initialise()
  {
    Serial.printf("BLEDevice1::initialise()\n");
  };
  virtual void connect()
  {
    Serial.printf("BLEDevice1::connect()\n");
  };
  virtual void onConnectedEvent(callBack ptr_onConnectedEvent)
  {
    _onConnectedEvent = ptr_onConnectedEvent;
  }
  virtual void onDisconnectedEvent(callBack ptr_onDisconnectedEvent)
  {
    _onDisconnectedEvent = ptr_onDisconnectedEvent;
  }
  virtual void fireEvents() {
    _onConnectedEvent();
    _onDisconnectedEvent();
  }
};

class ESPNowDevice : public IDevice
{
  typedef void (*callBack)();

public:
  virtual void initialise()
  {
    Serial.printf("ESPNowDevice::initialise()\n");
  };
  virtual void connect()
  {
    Serial.printf("ESPNowDevice::connect()\n");
  };
  virtual void onConnectedEvent(callBack ptr_onConnectedEvent)
  {
    _onConnectedEvent = ptr_onConnectedEvent;
  }
  virtual void onDisconnectedEvent(callBack ptr_onDisconnectedEvent)
  {
    _onDisconnectedEvent = ptr_onDisconnectedEvent;
  }
  virtual void fireEvents() {
    _onConnectedEvent();
    _onDisconnectedEvent();
  }
};
