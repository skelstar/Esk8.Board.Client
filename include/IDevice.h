#include <Arduino.h>

class IDevice
{

public:
  typedef void (*callBack)();
 
  virtual void initialise();
  virtual void connect() = 0;

  virtual void setOnConnectedEvent(callBack ptr_onConnectedEvent);
  virtual void setOnDisconnectedEvent(callBack ptr_onDisconnectedEvent);
  virtual void setOnNotifyEvent(callBack ptr_onNotifyEvent);

// protected:
  
  callBack _onConnectedEvent;
  callBack _onDisconnectedEvent;
  callBack _onNotifyEvent;
};

