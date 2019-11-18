#include "BLEDevice.h"
#ifndef IDevice
#include "IDevice.h"
#endif


class MyClientCallback : public BLEClientCallbacks
{
  private:
    void onConnect(BLEClient *pclient);
    void onDisconnect(BLEClient *pclient);
};

class MyBLEClient : public IDevice 
{
  public:

    void bleConnectedLocal() {
      // IDevice::_onConnectedEvent();
    };

    void bleDisconnectedLocal() {

    }
    MyBLEClient() {}

    virtual void initialise() 
    {
      Serial.printf("MyBLEClient initialise()\n");
    }
    virtual void connect() 
    {
    }
    virtual void setOnConnectedEvent(callBack ptr_onConnectEvent) 
    {
      // _onConnectedEvent2 = ptr_onConnectEvent;
    }
    virtual void setOnDisconnectedEvent(callBack ptr_onDisconnectedEvent)
    {
      _onDisconnectedEvent = ptr_onDisconnectedEvent;
    }
    virtual void setOnNotifyEvent(callBack ptr_onNotifyEvent) {
      _onNotifyEvent = ptr_onNotifyEvent;
    }

  // private:

    static BLEAddress *pServerAddress;
    static BLERemoteCharacteristic *pRemoteCharacteristic;

    static void notifyCallback(
      BLERemoteCharacteristic *pBLERemoteCharacteristic,
      uint8_t *pData,
      size_t length,
      bool isNotify)
    {
      memcpy(&vescdata, pData, sizeof(vescdata));
      // _onNotifyEvent();
    }

    bool bleConnectToServer()
    {
      BLEDevice::init("Esk8.Board.Client");
      pServerAddress = new BLEAddress(SERVER_ADDRESS);
      delay(200);
      BLEClient *pClient = BLEDevice::createClient();
      pClient->setClientCallbacks(new MyClientCallback());
      pClient->connect(*pServerAddress);
      delay(500);
      BLERemoteService *pRemoteService = pClient->getService(SERVICE_UUID);
      pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
      if (pRemoteCharacteristic->canNotify())
      {
        pRemoteCharacteristic->registerForNotify(notifyCallback);
      }
      return true;
    }
};

MyBLEClient client;
void onConnect(BLEClient *pclient)
{
  Serial.printf("Connected!\n");
  client._onConnectedEvent();
}

void onDisconnect(BLEClient *pclient)
{
  client._onDisconnectedEvent();
}

