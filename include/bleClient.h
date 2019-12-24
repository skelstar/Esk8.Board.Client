#include "BLEDevice.h"

/* ---------------------------------------------- */
static BLEAddress *pServerAddress;
static BLERemoteCharacteristic *pRemoteCharacteristic;

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pclient)
  {
    bleConnected();
  }

  void onDisconnect(BLEClient *pclient)
  {
    bleDisconnected();
  }
};

static void notifyCallback(
  BLERemoteCharacteristic *pBLERemoteCharacteristic,
  uint8_t *pData,
  size_t length,
  bool isNotify)
{
  memcpy(&vescdata, pData, sizeof(vescdata));
  bleReceivedNotify();
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

void send_packet_to_board()
{
  uint8_t bs[sizeof(ControllerData)];
  memcpy(bs, &controller_packet, sizeof(ControllerData));
  bool success = true;
  pRemoteCharacteristic->writeValue(bs, sizeof(ControllerData), true);
  if (success)
  {
    controller_packet.id = controller_packet.id + 1;
  }
  controller_packet.command = 0;  // clear command for next time
}
