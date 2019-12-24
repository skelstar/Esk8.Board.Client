#include <rom/rtc.h>

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
  // u8g2.setPowerSave(1);
  delay(500);
  pureDeepSleep();
}

void print_reset_reason(uint8_t cpu_core)
{
  RESET_REASON reason = rtc_get_reset_reason(cpu_core);
  switch ( reason )
  {
    case POWERON_RESET : Serial.printf("POWERON_RESET");break;          /**<1, Vbat power on reset*/
    case 2: Serial.println("DONT_KNOW");break;
    case SW_RESET : Serial.printf("SW_RESET");break;               /**<3, Software reset digital core*/
    case OWDT_RESET : Serial.printf("OWDT_RESET");break;             /**<4, Legacy watch dog reset digital core*/
    case DEEPSLEEP_RESET : Serial.printf("DEEPSLEEP_RESET");break;        /**<5, Deep Sleep reset digital core*/
    case SDIO_RESET : Serial.printf("SDIO_RESET");break;             /**<6, Reset by SLC module, reset digital core*/
    case TG0WDT_SYS_RESET : Serial.printf("TG0WDT_SYS_RESET");break;       /**<7, Timer Group0 Watch dog reset digital core*/
    case TG1WDT_SYS_RESET : Serial.printf("TG1WDT_SYS_RESET");break;       /**<8, Timer Group1 Watch dog reset digital core*/
    case RTCWDT_SYS_RESET : Serial.printf("RTCWDT_SYS_RESET");break;       /**<9, RTC Watch dog Reset digital core*/
    case INTRUSION_RESET : Serial.printf("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case TGWDT_CPU_RESET : Serial.printf("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case SW_CPU_RESET : Serial.printf("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case RTCWDT_CPU_RESET : Serial.printf("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case EXT_CPU_RESET : Serial.printf("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case RTCWDT_BROWN_OUT_RESET : Serial.printf("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case RTCWDT_RTC_RESET : Serial.printf("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : Serial.printf("NO_MEAN");
  }
}