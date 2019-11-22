#ifndef IEsk8Device
#include <IEsk8Device.h>
#endif
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

void initESPNow();

class EspNowClient : public IEsk8Device
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
  void setOnSentEvent(callBack ptr_onSentEvent) {
    _onSentEvent = ptr_onSentEvent;
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
// callback when data is sent from Master to Slave
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  client._onSentEvent();
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
  esp_now_register_send_cb(onDataSent);
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