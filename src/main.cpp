#define DEBUG_OUT Serial
#define PRINTSTREAM_FALLBACK
#include "Debug.hpp"

#include <Arduino.h>
#include <Wire.h>
#include <myPushButton.h>
#include <VescData.h>
#include <Fsm.h>
#include <LogansGreatButton.h>

//======================================
#define ESP32_MINI "80:7D:3A:C5:6A:36"
#define TTGO_T_DISPLAY_SERVER_ADDR "84:0D:8E:3B:91:3E"
#define TTGO_ESP32_OLED_V2_0 "80:7D:3A:B9:A8:6A"
#define ESP32_MINI_B "80:7D:3A:C4:50:9A"
#define ESP32_MINI_C "3C:71:BF:F0:C5:4A"
#define BLE_M5STICK   "3C:71:BF:45:FE:16"
#define ESP32_MINI_WITH_BATT_CONNECTOR "80:7D:3A:C5:62:96"
//--------------------------------------
#define SERVER_ADDRESS ESP32_MINI_WITH_BATT_CONNECTOR
//======================================

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define BATTERY_VOLTAGE_FULL 4.2 * 11         // 46.2
#define BATTERY_VOLTAGE_CUTOFF_START 3.4 * 11 // 37.4
#define BATTERY_VOLTAGE_CUTOFF_END 3.1 * 11   // 34.1
/* ---------------------------------------------- */

static boolean serverConnected = false;

#define CHECK_BATT_VOLTS 1
#define CHECK_AMP_HOURS 2
#define CHECK_MOTOR_CURRENT 3
#define CHECK_MOVING 4

#define BUTTON_0_PIN 0

VescData vescdata, old_vescdata;

#include "utils.h"
#include "TTGO_T_Display.h"
#include "screens.h"
#include "stateMachine.h"

void bleConnected()
{
  Serial.printf("serverConnected! \n");
  serverConnected = true;
  fsm.trigger(EV_SERVER_CONNECTED);
}

void bleDisconnected()
{
  serverConnected = false;
  Serial.printf("disconnected!");
  fsm.trigger(EV_SERVER_DISCONNECTED);
}

void bleReceivedNotify()
{
  Serial.printf("Received: %.1fV %.1fmAh %.1fm \n", vescdata.batteryVoltage, vescdata.ampHours, vescdata.odometer);
  if (vescdata.moving != old_vescdata.moving)
  {
    TRIGGER(vescdata.moving ? EV_MOVING : EV_STOPPED_MOVING);
  }
  memcpy(&old_vescdata, &vescdata, sizeof(vescdata));
}

#include "bleClient.h"
/* ---------------------------------------------- */

#define OFFSTATE HIGH

#include "buttons.h"

/*------------------------------------------------------------------*/
void setup()
{
  Wire.begin(21, 22, 100000);

  Serial.begin(115200);
  Serial.println("\nStarting Arduino BLE Client application...");

  button0_initialise();

  display_initialise();

  addFsmTransitions();
  fsm.run_machine();

  if (serverConnected == false)
  {
    Serial.printf("Trying to connect to server\n");
    serverConnected = bleConnectToServer();
  }
}

void loop()
{
  button0.LOOPButtonController();

  fsm.run_machine();

  delay(10);
}
