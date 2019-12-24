#define DEBUG_OUT Serial
#define PRINTSTREAM_FALLBACK
#include "Debug.hpp"

#include <Arduino.h>
#include <Wire.h>
#include <myPushButton.h>
#include <VescData.h>
#include <Fsm.h>
#include <LogansGreatButton.h>
#include <TaskScheduler.h>
#include <elapsedMillis.h>

//======================================
// #define ESP32_MINI "80:7D:3A:C5:6A:36"
// #define TTGO_T_DISPLAY_SERVER_ADDR "84:0D:8E:3B:91:3E"
// #define TTGO_ESP32_OLED_V2_0 "80:7D:3A:B9:A8:6A"
// #define ESP32_MINI_B "80:7D:3A:C4:50:9A"
// #define ESP32_MINI_C "3C:71:BF:F0:C5:4A"
// #define BLE_M5STICK   "3C:71:BF:45:FE:16"

#define ESP32_MINI_IN_BOARD "80:7D:3A:C5:6B:0E"
#define ESP32_MINI_WITH_BATT_CONNECTOR "80:7D:3A:C5:62:96"
//--------------------------------------
#define SERVER_ADDRESS ESP32_MINI_IN_BOARD
//======================================

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define BATTERY_VOLTAGE_FULL 4.2 * 11         // 46.2
#define BATTERY_VOLTAGE_CUTOFF_START 3.4 * 11 // 37.4
#define BATTERY_VOLTAGE_CUTOFF_END 3.1 * 11   // 34.1
/* ---------------------------------------------- */


/* prototypes */
void bleConnected();
void bleDisconnected();
void bleReceivedNotify();



#define USE_TEST_VALUES
#ifdef USE_TEST_VALUES
#define SEND_TO_BOARD_INTERVAL 200

#else
#define SEND_TO_BOARD_INTERVAL 200
#endif


static boolean serverConnected = false;
uint16_t missing_packet_count = 0;

#define BUTTON_0_PIN 0

VescData vescdata, old_vescdata;
ControllerData controller_packet;

#define STORE_NAMESPACE "ESK8"
#define STORE_KEY_CONNECT_COUNT "CONNECT_COUNT"
#define STORE_KEY_MISSING_COUNT "MISSING_COUNT"


#include "utils.h"
#include "storage.h"
#include "TTGO_T_Display.h"
#include "screens.h"
#include "bleClient.h"
#include "stateMachine.h"


//--------------------------------------

void bleConnected()
{
  fsm.trigger(EV_SERVER_CONNECTED);
}

void bleDisconnected()
{
  fsm.trigger(EV_SERVER_DISCONNECTED);
}

void bleReceivedNotify()
{
  if (vescdata.reason == ReasonType::REQUESTED)
  {
    DEBUG("REQUESTED");
    // TRIGGER(EV_UPDATE_FROM_BOARD);
  }
  else if (vescdata.reason == ReasonType::FIRST_PACKET)
  {
    DEBUG("FIRST_PACKET");
    TRIGGER(EV_UPDATE_FROM_BOARD);
  }
  Serial.printf("Received: %.1fV %.1fmAh %.1fm \n", vescdata.batteryVoltage, vescdata.ampHours, vescdata.odometer);
  if (vescdata.moving != old_vescdata.moving)
  {
    TRIGGER(vescdata.moving ? EV_MOVING : EV_STOPPED_MOVING);
  }
  memcpy(&old_vescdata, &vescdata, sizeof(vescdata));
}

/* ---------------------------------------------- */

#define OFFSTATE HIGH

#include "buttons.h"

/* ---------------------------------------------- */

elapsedMillis since_last_request = 0;

Scheduler runner;

Task t_SendToBoard(
    SEND_TO_BOARD_INTERVAL,
    TASK_FOREVER,
    [] {
      if (since_last_request > 5000)
      {
        since_last_request = 0;
        // controller_packet.command = COMMAND_REQUEST_UPDATE;
      }
      send_packet_to_board();
    });

/*------------------------------------------------------------------*/
void setup()
{
  Wire.begin(21, 22, 100000);

  Serial.begin(115200);
  Serial.println("\nStarting Arduino BLE Client application...");

  reset_reason = rtc_get_reset_reason(0);

  Serial.printf("Reset reason (Core0): "); print_reset_reason(0); Serial.printf("\n");
  Serial.printf("Reset reason (Core1): "); print_reset_reason(1); Serial.printf("\n");

  if (rtc_get_reset_reason(1) == SW_CPU_RESET)
  {
    DEBUG("### BLE Disconnect reset CPU");
  }

  controller_packet.id = 0;

  runner.startNow();
  runner.addTask(t_SendToBoard);

  t_SendToBoard.enable();

  button0_initialise();

  display_initialise();

  addFsmTransitions();
  fsm.run_machine();
}

void loop()
{
  button0.LOOPButtonController();

  fsm.run_machine();
  
  runner.execute();

  delay(10);
}
