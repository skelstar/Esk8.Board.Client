

uint8_t getBatteryPercentage(float voltage) {
  float voltsLeft = voltage - BATTERY_VOLTAGE_CUTOFF_END;
  float voltsAvail = BATTERY_VOLTAGE_FULL - BATTERY_VOLTAGE_CUTOFF_END;

  uint8_t percent = 0;
  if ( voltage > BATTERY_VOLTAGE_CUTOFF_END ) { 
    percent = (voltsLeft /  voltsAvail) * 100;
  }
  if (percent > 100) {
    percent = 100;
	}
  return percent;
}


void pureDeepSleep()
{
  // https://esp32.com/viewtopic.php?t=3083
  // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);

  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  //   IMU.setSleepEnabled(true);
  delay(100);
  //adc_power_off();
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, LOW); //1 = High, 0 = Low
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void deepSleep()
{
  // WiFi.mode(WIFI_OFF);  // wifi
  btStop();           // ble
  u8g2.setPowerSave(1);
  delay(500);
  pureDeepSleep();
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

bool connectToWifi() {
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();

  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration'

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("esk8.board");

  Serial.printf("Connected to wifi!\n");

  return true;
  // if (!wifiManager.startConfigPortal("esk8.board")) {
  //     Serial.println("failed to connect and hit timeout");
  //     delay(5000);
  //     //reset and try again, or maybe put it to deep sleep
  //     ESP.restart();
  //     delay(5000);
  //   }
  // wifiManager.autoConnect("esk8.board");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  }
