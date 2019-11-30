#include <Arduino.h>

#define PRINTSTREAM_FALLBACK 1
#define DEBUG_OUT Serial
#include "Debug.hpp"

#include <Wire.h>
#include <myPushButton.h>
#include <VescData.h>
#include <Fsm.h>
#include <LogansGreatButton.h>

#define IS_CONTROLLER 1

#include <nrf24L01Client.h>

#define BATTERY_VOLTAGE_FULL 4.2 * 11         // 46.2
#define BATTERY_VOLTAGE_CUTOFF_START 3.4 * 11 // 37.4
#define BATTERY_VOLTAGE_CUTOFF_END 3.1 * 11   // 34.1
/* ---------------------------------------------- */

#define CHECK_BATT_VOLTS 1
#define CHECK_AMP_HOURS 2
#define CHECK_MOTOR_CURRENT 3
#define CHECK_MOVING 4

#define BUTTON_PIN 0 // M5Stick pin 35

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

unsigned long lastPacketId = 0;
float missedPacketCounter = 0.0;
unsigned long sendCounter = 0;
bool syncdWithServer = false;

#include "display.h"
#include "display-rangereport.h"
#include "utils.h"
#include "stateMachine.h"

void sentToDevice()
{
}

void packetReceived(const void *data, uint8_t data_len)
{
  BoardPacket recv_pkt;
  memcpy(/*dest*/ &recv_pkt, /*src*/ data, data_len);

  DEBUGVAL(recv_pkt.id, sendCounter, lastPacketId);

  if (lastPacketId != recv_pkt.id - 1)
  {
    if (syncdWithServer)
    {
      uint8_t lost = (recv_pkt.id - 1) - lastPacketId;
      missedPacketCounter = missedPacketCounter + lost;
      Serial.printf("Missed %d packets! (%.0f total)\n", lost, missedPacketCounter);
      vescdata.ampHours = missedPacketCounter;
      fsm.trigger(EV_RECV_PACKET);
    }
  }
  else
  {
    syncdWithServer = true;
  }

  lastPacketId = recv_pkt.id;
}

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
  Serial.println("\nStarting Missing packet report client...");

  button.onPressShortRelease(onButtonPressShortRelease);
  button.onPressLongStart(onButtonPressLongStart);
  button.onPressLongRelease(onButtonPressLongRelease);
  button.onHoldStart(onButtonHoldStart);
  button.onHoldContinuous(onButtonHoldContinuous);
  button.onHoldRelease(onButtonHoldRelease);

  vescdata.ampHours = 0.0;
  vescdata.batteryVoltage = 0.0;
  vescdata.odometer = 0.0;

  addFsmTransitions();
  fsm.run_machine();

  client.setOnConnectedEvent([] {
    Serial.printf("Connected!\n");
  });
  client.setOnDisconnectedEvent([] {
    Serial.println("Init Failed, restarting...");
    syncdWithServer = false;
  });
  client.setOnNotifyEvent(packetReceived);
  client.setOnSentEvent(sentToDevice);
  client.initialise();
  initNRF24L01(nrf24.RF24_CLIENT);
}

unsigned long now = 0;

void loop()
{
  button.LOOPButtonController();

  handleBoardMovingStopping();

  fsm.run_machine();

  client.update();

  if (millis() - now > 1000)
  {
    now = millis();

    nrf24.send_packet.id = sendCounter++;

    bool success = sendPacket();
    if (!success)
    {
      DEBUG("sendPacket() failed!");
    }
  }
  delay(10);
}
