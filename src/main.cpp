#include <Arduino.h>
#include <Wire.h>
#include <myPushButton.h>
#include <VescData.h>

#include <Fsm.h>

#define ESP32_MINI "80:7D:3A:C5:6A:36"
#define TTGO_T_DISPLAY_SERVER_ADDR "84:0D:8E:3B:91:3E"
#define TTGO_ESP32_OLED_V2_0 "80:7D:3A:B9:A8:6A"
#define ESP32_MINI_B "80:7D:3A:C4:50:9A"
#define ESP32_MINI_C "3C:71:BF:F0:C5:4A"
#define SERVER_ADDRESS  ESP32_MINI_C

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define BATTERY_VOLTAGE_FULL          4.2 * 11      // 46.2
#define BATTERY_VOLTAGE_CUTOFF_START  3.4 * 11      // 37.4
#define BATTERY_VOLTAGE_CUTOFF_END    3.1 * 11      // 34.1

#define LED_ON HIGH
#define LED_OFF LOW

#define STICK_LED_PIN     19
#define STICK_IR_PIN      17
#define STICK_BUZZER_PIN  26
#define STICK_BUTTON_PIN  35

/* ---------------------------------------------- */

static boolean serverConnected = false;

#define STATE_POWER_UP 0
#define STATE_CONNECTING 1
#define STATE_CONNECTED 2
#define STATE_BATTERY_VOLTAGE_SCREEN 3

#define CHECK_BATT_VOLTS  1
#define CHECK_AMP_HOURS   2
#define CHECK_MOTOR_CURRENT 3

VescData vescdata, oldvescdata;

bool valueChanged(uint8_t measure) {
  switch (measure) {
    case CHECK_BATT_VOLTS:
      if (oldvescdata.batteryVoltage != vescdata.batteryVoltage) {
        oldvescdata.batteryVoltage = vescdata.batteryVoltage;
        return true;
      }
      return false;
    case CHECK_AMP_HOURS:
      if (oldvescdata.ampHours != vescdata.ampHours) {
        oldvescdata.ampHours = vescdata.ampHours;
        return true;
      }
      return false;
    case CHECK_MOTOR_CURRENT:
      return true;
  }
  return false;
}


xQueueHandle xQueue;
const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
#define OTHER_CORE 0

#include "display.h"
#include "utils.h"
#include "stateMachine.h"

void bleConnected()
{
  Serial.printf("serverConnected! \n");
  serverConnected = true;
  fsm.trigger(SERVER_CONNECTED);
}

void bleDisconnected()
{
  serverConnected = false;
  Serial.printf("disconnected!");
  fsm.trigger(SERVER_DISCONNECTED);
}

void bleReceivedNotify()
{
  Serial.printf("Received: %.1fV %.1fmAh %.1fm \n", vescdata.batteryVoltage, vescdata.ampHours, vescdata.odometer);
}

#include "bleClient.h"
/* ---------------------------------------------- */

#define OFFSTATE HIGH

void sendClearTripOdoToMonitor();
void checkBoardMoving();
void buzzerBuzz();
void setupPeripherals();

void listener_Button(int eventCode, int eventPin, int eventParam);
myPushButton button(STICK_BUTTON_PIN, PULLUP, OFFSTATE, [](int eventCode, int eventPin, int eventParam)
  {
    const int powerDownOption = 2;

    switch (eventCode)
    {
      case button.EV_BUTTON_PRESSED:
        break;
 
      case button.EV_HELD_SECONDS:
        switch (eventParam) {
          case powerDownOption:   // power down
            fsm.trigger(EV_HELD_POWER_OFF_OPTION);
            break;
          default:
            fsm.trigger(EV_HELD_DOWN_WAIT);
            break;
        }
        break;

      case button.EV_RELEASED:
        switch (eventParam) {
          case powerDownOption:
            deepSleep();
            break;
          default:
            if (eventParam < 1) {
              fsm.trigger(BUTTON_CLICK);
            }
            else {
              fsm.trigger(EV_NO_HELD_OPTION_SELECTED);
            }
            break;
          }
        break;
      case button.EV_DOUBLETAP:
        break;
    }
  });

void checkBoardMoving() {
  if (oldvescdata.moving != vescdata.moving)
  {
    oldvescdata.moving = vescdata.moving;
    if (vescdata.moving)
    {
      fsm.trigger(MOVING);
    }
    else
    {
      fsm.trigger(STOPPED_MOVING);
    }
  }
}

void buzzerBuzz()
{
  for (int i = 0; i < 100; i++)
  {
    digitalWrite(STICK_BUZZER_PIN, HIGH);
    delay(1);
    digitalWrite(STICK_BUZZER_PIN, LOW);
    delay(1);
  }
}

void setupPeripherals()
{
  pinMode(STICK_LED_PIN, OUTPUT);
  pinMode(STICK_IR_PIN, OUTPUT);
  pinMode(STICK_BUZZER_PIN, OUTPUT);
  digitalWrite(STICK_LED_PIN, LED_ON);
  digitalWrite(STICK_BUZZER_PIN, LOW);
  u8g2.setFont(u8g2_font_4x6_tr);
}
/*------------------------------------------------------------------*/ 
void setup()
{
  Wire.begin(21, 22, 100000);
  u8g2.begin();

  Serial.begin(115200);
  Serial.println("\nStarting Arduino BLE Client application...");

  addFsmTransitions();
  fsm.run_machine();

  setupPeripherals();

  // if button held then we can shut down
  button.serviceEvents();
  while (button.isPressed())
  {
    fsm.run_machine();
    button.serviceEvents();
  }
  button.serviceEvents();
}

BaseType_t xStatus;

void loop()
{
  button.serviceEvents();

  checkBoardMoving();

  fsm.run_machine();

  if (serverConnected == false)
  {
    Serial.printf("Trying to connect to server\n");
    serverConnected = bleConnectToServer();
  }

  delay(10);
}

