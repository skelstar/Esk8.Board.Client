#include "BLEDevice.h"

/* ---------------------------------------------- */
static BLEAddress *pServerAddress;
static BLERemoteCharacteristic *pRemoteCharacteristic;

class MyClientCallback : public BLEClientCallbacks
{
  public:
    void onConnect(BLEClient *pclient);
    void onDisconnect(BLEClient *pclient);
};

class MyBLEClient : public IEsk8Device {

  public:
    MyBLEClient() {
    }

    void initialise() {
      Serial.printf("MyBLEClient Initialised()! \n");
    }

    void connect() {
      Serial.printf("MyBLEClientconnect()! \n");
      _onConnectedEvent();
    }

    static void notifyCallback(
      BLERemoteCharacteristic *pBLERemoteCharacteristic,
      uint8_t *pData,
      size_t length,
      bool isNotify)
    {
      memcpy(&vescdata, pData, sizeof(vescdata));
      bleReceivedNotify();
    }

    void setOnConnectedEvent(callBack ptr_onConnectedEvent) {
      _onConnectedEvent = ptr_onConnectedEvent;
    }
    void setOnDisconnectedEvent(callBack ptr_onDisconnectedEvent) {
    }
    void setOnNotifyEvent(callBack ptr_onNotifyEvent) {
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

#ifndef myBleClient
MyBLEClient myBleClient;
#endif

void MyClientCallback::onConnect(BLEClient *pclient)
{
  myBleClient._onConnectedEvent();
}

void MyClientCallback::onDisconnect(BLEClient *pclient)
{
  myBleClient._onDisconnectedEvent();
}
