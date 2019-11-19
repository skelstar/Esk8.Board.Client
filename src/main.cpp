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
//--------------------------------------
#define SERVER_ADDRESS BLE_M5STICK
//======================================

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define BATTERY_VOLTAGE_FULL 4.2 * 11         // 46.2
#define BATTERY_VOLTAGE_CUTOFF_START 3.4 * 11 // 37.4
#define BATTERY_VOLTAGE_CUTOFF_END 3.1 * 11   // 34.1
/* ---------------------------------------------- */

static boolean serverConnected = false;

#define STATE_POWER_UP 0
#define STATE_CONNECTING 1
#define STATE_CONNECTED 2
#define STATE_BATTERY_VOLTAGE_SCREEN 3

#define CHECK_BATT_VOLTS 1
#define CHECK_AMP_HOURS 2
#define CHECK_MOTOR_CURRENT 3
#define CHECK_MOVING 4

#define BUTTON_PIN 0

VescData vescdata, oldvescdata;

bool changed(uint8_t metric)
{
  bool valChanged = false;
  switch (metric)
  {
  case CHECK_BATT_VOLTS:
    valChanged = oldvescdata.batteryVoltage != vescdata.batteryVoltage;
    oldvescdata.batteryVoltage = vescdata.batteryVoltage;
    return valChanged;
  case CHECK_AMP_HOURS:
    valChanged = oldvescdata.ampHours != vescdata.ampHours;
    oldvescdata.ampHours = vescdata.ampHours;
    return valChanged;
  case CHECK_MOVING:
    valChanged = oldvescdata.moving != vescdata.moving;
    oldvescdata.moving = vescdata.moving;
    return valChanged;
  default:
    Serial.printf("WARNING: Unhandled changed value %d\n", metric);
    return false;
  }
  return false;
}

#include "display.h"
#include "utils.h"
#include "stateMachine.h"

#include "testIDeviceClass.h"

TestIDeviceClass testIDevice;

void bleConnected()
{
}

void bleDisconnected()
{
}

void bleReceivedNotify()
{
  Serial.printf("Received: %.1fV %.1fmAh %.1fm \n", vescdata.batteryVoltage, vescdata.ampHours, vescdata.odometer);
}

#include "bleClient.h"

/* ---------------------------------------------- */

#define OFFSTATE HIGH

void sendClearTripOdoToMonitor();
void handleBoardMovingStopping();

#include "buttons.h"

void handleBoardMovingStopping()
{

  if (changed(CHECK_MOVING))
  {
    fsm.trigger(vescdata.moving ? MOVING : STOPPED_MOVING);
  }
}
/*------------------------------------------------------------------*/
void setup()
{
  Wire.begin(21, 22, 100000);
  u8g2.begin();

  Serial.begin(115200);
  Serial.println("\nStarting Arduino BLE Client application...");

  button.onPressShortRelease(onButtonPressShortRelease);
  button.onPressLongStart(onButtonPressLongStart);
  button.onPressLongRelease(onButtonPressLongRelease);
  button.onHoldStart(onButtonHoldStart);
  button.onHoldContinuous(onButtonHoldContinuous);
  button.onHoldRelease(onButtonHoldRelease);

  addFsmTransitions();
  fsm.run_machine();

  myBleClient.initialise();
  myBleClient.setOnConnectedEvent([] {
    Serial.printf("myBleClient.setOnConnectedEvent()! \n");
    serverConnected = true;
    fsm.trigger(SERVER_CONNECTED);
  });
  myBleClient.setOnDisconnectedEvent([] {
    serverConnected = false;
    Serial.printf("myBleClient.setOnDisconnectedEvent() disconnected!");
    fsm.trigger(SERVER_DISCONNECTED);
  });

  if (serverConnected == false)
  {
    Serial.printf("Trying to connect to server\n");
    serverConnected = myBleClient.bleConnectToServer();
  }

  // testIDevice.setOnConnectedEvent([] {
  //   Serial.printf("testIDevice.setOnConnectedEvent() triggered\n");
  // });
  // testIDevice.connect();
}

void loop()
{
  button.LOOPButtonController();

  handleBoardMovingStopping();

  fsm.run_machine();

  delay(10);
}
