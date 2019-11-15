#include <Arduino.h>

class IDevice
{
  typedef void (*callBack)();

public:
  virtual void initialise();
  virtual void connect() = 0;
  void onConnectedEvent(callBack ptr_onConnectedEvent)
  {
    _onConnectedEvent = ptr_onConnectedEvent;
  }
  void onDisconnectedEvent(callBack ptr_onDisconnectedEvent)
  {
    _onDisconnectedEvent = ptr_onDisconnectedEvent;
  }
  void fireEvents()
  {
    _onConnectedEvent();
    _onDisconnectedEvent();
  }

// public:
  callBack _onConnectedEvent;
  callBack _onDisconnectedEvent;
};

class BLEDevice1 : public IDevice
{
public:
  virtual void initialise()
  {
    Serial.printf("BLEDevice1::initialise()\n");
  };
  virtual void connect()
  {
    Serial.printf("BLEDevice1::connect()\n");
  };
};

class ESPNowDevice : public IDevice
{
public:
  virtual void initialise()
  {
    Serial.printf("ESPNowDevice::initialise()\n");
  };
  virtual void connect()
  {
    Serial.printf("ESPNowDevice::connect()\n");
  };
};
