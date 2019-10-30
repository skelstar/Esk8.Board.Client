#include <Arduino.h>
#include <Wire.h>
#include <myPushButton.h>
#include <VescData.h>

#include <Fsm.h>

#define ESP32_MINI "80:7D:3A:C5:6A:36"
#define TTGO_T_DISPLAY_SERVER_ADDR "84:0D:8E:3B:91:3E"
#define TTGO_ESP32_OLED_V2_0 "80:7D:3A:B9:A8:6A"
#define ESP32_MINI_B "80:7D:3A:C4:50:9A"
#define SERVER_ADDRESS  ESP32_MINI_B

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

enum EventsEnum
{
  BUTTON_CLICK,
  SERVER_CONNECTED,
  SERVER_DISCONNECTED,
  MOVING,
  STOPPED_MOVING,
  HELD_POWERDOWN_WINDOW,
  HELD_CLEAR_TRIP_WINDOW,
  BUTTON_BEING_HELD,
  SENT_CLEAR_TRIP_ODO,
  HELD_RELEASED
} event;

//-------------------------------
State state_connecting(
  [] { 
    lcdConnectingPage("connecting...", vescdata.ampHours, vescdata.odometer);
  }, 
  NULL, 
  NULL
);
//-------------------------------
State state_connected(
  NULL,
  [] { drawBattery(getBatteryPercentage(vescdata.batteryVoltage), valueChanged(CHECK_BATT_VOLTS)); }, 
  NULL
);
//-------------------------------
State state_server_disconnected(
  [] { lcdConnectingPage("disconnected", vescdata.ampHours, vescdata.odometer); }, 
  NULL, 
  NULL
);
//-------------------------------
State state_battery_voltage_screen(
  [] { drawBattery(getBatteryPercentage(vescdata.batteryVoltage), true); },
  [] { drawBattery(getBatteryPercentage(vescdata.batteryVoltage), valueChanged(CHECK_BATT_VOLTS)); },
  NULL
);
//-------------------------------
State state_trip_page(
  [] { lcdTripPage(vescdata.ampHours, vescdata.odometer, vescdata.vescOnline, true); }, 
  [] { lcdTripPage(vescdata.ampHours, vescdata.odometer, vescdata.vescOnline, valueChanged(CHECK_AMP_HOURS)); }, 
  NULL
);
//-------------------------------
State state_moving_screen(
  [] { clearScreen(); }, 
  NULL, 
  NULL
);
//-------------------------------
State state_button_being_held(
  [] { lcdMessage("..."); Serial.printf("state_button_being_held\n"); }, 
  NULL, 
  NULL
);
//-------------------------------
State state_button_held_powerdown_window(
  [] { lcdMessage("power down?"); }, 
  NULL, 
  NULL
);
//-------------------------------

Fsm fsm(&state_connecting);

void addFsmTransitions() {

  uint8_t event = SERVER_DISCONNECTED;
  fsm.add_transition(&state_battery_voltage_screen, &state_server_disconnected, event, NULL);
  fsm.add_transition(&state_trip_page, &state_server_disconnected, event, NULL);

  event = SERVER_CONNECTED;
  fsm.add_transition(&state_connecting, &state_connected, event, NULL);
  // when state connected is entered it will transition to new state after 3 seconds
  fsm.add_timed_transition(&state_connected, &state_trip_page, 3000, NULL);

  event = BUTTON_CLICK;
  fsm.add_transition(&state_battery_voltage_screen, &state_trip_page, event, NULL);
  fsm.add_transition(&state_trip_page, &state_battery_voltage_screen, event, NULL);
  fsm.add_transition(&state_button_being_held, &state_trip_page, event, NULL);

  event = MOVING;
  fsm.add_transition(&state_trip_page, &state_moving_screen, event, NULL);

  event = STOPPED_MOVING;
  fsm.add_transition(&state_moving_screen, &state_trip_page, event, NULL);

  event = BUTTON_BEING_HELD;
  fsm.add_transition(&state_connecting, &state_button_being_held, event, NULL);
  fsm.add_transition(&state_battery_voltage_screen, &state_button_being_held, event, NULL);
  fsm.add_transition(&state_trip_page, &state_button_being_held, event, NULL);

  event = HELD_POWERDOWN_WINDOW;
  fsm.add_transition(&state_button_being_held, &state_button_held_powerdown_window, event, NULL);

  event = HELD_CLEAR_TRIP_WINDOW;

  event = SENT_CLEAR_TRIP_ODO;
}
/* ---------------------------------------------- */

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
    bool sleepTimeWindow = eventParam >= 2 && eventParam <= 3;
    // bool clearTripWindow = eventParam >= 4 && eventParam <= 5;

    switch (eventCode)
    {
      case button.EV_BUTTON_PRESSED:
        break;
 
      case button.EV_HELD_SECONDS:
        if (sleepTimeWindow)
        {
          fsm.trigger(HELD_POWERDOWN_WINDOW);
        }
        else
        {
          fsm.trigger(BUTTON_BEING_HELD);
        }
        break;

      case button.EV_RELEASED:
        if (sleepTimeWindow)
        {
          deepSleep();
          break;
        }
        else
        {
          fsm.trigger(BUTTON_CLICK);
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

#define CLEAR_TRIP_ODO_COMMAND 99

void sendClearTripOdoToMonitor()
{
  Serial.printf("sending clear trip odo command to master\n");
  pRemoteCharacteristic->writeValue(CLEAR_TRIP_ODO_COMMAND, sizeof(uint8_t));
  buzzerBuzz();
  fsm.trigger(SENT_CLEAR_TRIP_ODO);
}

/*------------------------------------------------------------------*/ 

void coreTask( void * pvParameters ){
 
  Serial.printf("coreTask running on Core %d\n", xPortGetCoreID());

  while(true){
    if (serverConnected == false)
    {
      Serial.printf("Trying to connect to server\n");
      serverConnected = bleConnectToServer();
      Serial.printf("connected? %d\n", serverConnected);
    }
    vTaskDelay(1);
  }
  vTaskDelete( NULL );
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

  
  xTaskCreatePinnedToCore(coreTask, "coreTask", 10000, NULL, /*priority*/ 0, NULL, OTHER_CORE);
  
  xQueue = xQueueCreate(1, sizeof(EventsEnum));

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

  EventsEnum e;
  xStatus = xQueueReceive(xQueue, &e, xTicksToWait);
  if (xStatus == pdPASS)
  {
    switch (e)
    {
    case SERVER_CONNECTED:
    case SERVER_DISCONNECTED:
      fsm.trigger(e);
      break;
    default:
      Serial.printf("Unhandled event code: %d \n", e);
    }
  }

  delay(10);
}

