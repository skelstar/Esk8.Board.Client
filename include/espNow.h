#ifndef IDevice
#include <IDevice.h>
#endif
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

// static void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
// {
//   _onNotifyEvent();
// }

void initESPNow();



class EspNowClient : public IDevice
{
public:
  EspNowClient()
  {
  }

  void initialise()
  {
    initESPNow();
  }

  void connect()
  {
  }

  void setOnConnectedEvent(callBack ptr_onConnectedEvent)
  {
    _onConnectedEvent = ptr_onConnectedEvent;
  }
  void setOnDisconnectedEvent(callBack ptr_onDisconnectedEvent)
  {
    _onDisconnectedEvent = ptr_onDisconnectedEvent;
  }
  void setOnNotifyEvent(callBack ptr_onNotifyEvent)
  {
    _onNotifyEvent = ptr_onNotifyEvent;
  }

private:
};


#ifndef espnowClient
EspNowClient client;
#endif

void configDeviceAP();

void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  client._onNotifyEvent();
}

// Init ESP Now with fallback
void initESPNow()
{
  WiFi.mode(WIFI_AP);
  configDeviceAP();
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK)
  {
    client._onConnectedEvent();
  }
  else
  {
    client._onDisconnectedEvent();
    ESP.restart();
  }

  esp_now_register_recv_cb(onDataRecv);
}

// config AP SSID
void configDeviceAP()
{
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result)
  {
    Serial.println("AP Config failed.");
  }
  else
  {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}