

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
