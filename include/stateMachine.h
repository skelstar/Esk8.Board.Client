
enum EventsEnum
{
  EV_SERVER_CONNECTED,
  EV_SERVER_DISCONNECTED,
  EV_MOVING,
  EV_STOPPED_MOVING,
  EV_BUTTON_CLICK,
  EV_UPDATE_FROM_BOARD,
} event;

enum StateId
{
  STATE_CONNECTED,
  STATE_DISCONNECTED,
  STATE_BATTERY_SCREEN,
  STATE_MAIN_PAGE,
  STATE_MOVING,
};

void triggerEvent(EventsEnum event);

//-------------------------------
State state_connecting(
    STATE_CONNECTED,
    [] {
      DEBUG("state_connecting -------------------");
      tft.fillScreen(TFT_BLACK);
      lcd_message_middle(MC_DATUM, "searching...", 2);

      if (serverConnected == false)
      {
        Serial.printf("Trying to connect to server\n");
        serverConnected = bleConnectToServer();
      }
    },
    NULL,
    [] {
      if (reset_reason == RESET_REASON::POWERON_RESET)
      {
        DEBUG("Stored: 1, Missing: 0");
        storeUInt8(STORE_NAMESPACE, STORE_KEY_CONNECT_COUNT, 1);
        storeUInt8(STORE_NAMESPACE, STORE_KEY_MISSING_COUNT, 0);
      }
      else
      {
        // connect
        uint8_t connect_count = recallUInt8(STORE_NAMESPACE, STORE_KEY_CONNECT_COUNT);
        connect_count = connect_count + 1;
        DEBUGVAL("Stored", connect_count);
        storeUInt8(STORE_NAMESPACE, STORE_KEY_CONNECT_COUNT, connect_count);
        // missing
        missing_packet_count = recallUInt8(STORE_NAMESPACE, STORE_KEY_MISSING_COUNT);
        DEBUGVAL(missing_packet_count);

        serverConnected = true;
      }
    });
//-------------------------------
State state_disconnected(
    STATE_DISCONNECTED,
    [] {
      serverConnected = false;
      DEBUG("state_disconnected -------------------");
      tft.fillScreen(TFT_BLACK);
      lcd_message_middle(MC_DATUM, "disconnected!!!", 2);
    },
    NULL,
    NULL);
//-------------------------------
State state_battery_voltage_screen(
    STATE_BATTERY_SCREEN,
    [] {
      DEBUG("state_battery_voltage_screen -------------------");
      drawBattery(getBatteryPercentage(vescdata.batteryVoltage));
    },
    NULL,
    NULL);
//-------------------------------
State state_trip_page(
    STATE_MAIN_PAGE,
    [] {
      DEBUG("state_trip_page -------------------");
      tft.fillScreen(TFT_BLACK);
      // connects
      lcd_message(TL_DATUM, "Connects: ", 0, 0, 2);
      int connects = recallUInt8(STORE_NAMESPACE, STORE_KEY_CONNECT_COUNT);
      char buff[3];
      sprintf(buff, "%02d", connects);
      chunkyDrawFloat(TR_DATUM, /*y*/0, /*num_chars*/2, buff, /*size*/2);

      uint8_t line_height = FONT_2_LINE_HEIGHT;
      // board missed : number times controller disconnected
      lcd_message(TL_DATUM, "Board missed: ", 0, line_height*2, 2);
      sprintf(buff, "%02.0f", vescdata.ampHours);
      chunkyDrawFloat(TR_DATUM, line_height*2, 2, buff, /*size*/2);
      DEBUGVAL(vescdata.ampHours);
    },
    NULL,
    NULL);
//-------------------------------
State state_moving_screen(
    STATE_MOVING,
    [] {
      DEBUG("state_moving_screen -------------------");
      tft.fillScreen(TFT_BLACK);
    },
    NULL,
    NULL);
//-------------------------------

Fsm fsm(&state_connecting);

//-------------------------------
void triggerEvent(EventsEnum event)
{
  fsm.trigger(event);
}
//-------------------------------

void addFsmTransitions()
{
  uint8_t event = EV_SERVER_DISCONNECTED;
  fsm.add_transition(&state_battery_voltage_screen, &state_disconnected, event, NULL);
  fsm.add_transition(&state_trip_page, &state_disconnected, event, NULL);
  fsm.add_timed_transition(&state_disconnected, &state_connecting, 100, NULL);

  event = EV_SERVER_CONNECTED;
  fsm.add_transition(&state_connecting, &state_trip_page, event, NULL);

  event = EV_BUTTON_CLICK;
  fsm.add_transition(&state_trip_page, &state_battery_voltage_screen, event, NULL);
  // and transition back

  fsm.add_transition(&state_disconnected, &state_battery_voltage_screen, event, NULL);
  // if going to batt screen, go back to trip page after 2 seconds

  fsm.add_timed_transition(&state_battery_voltage_screen, &state_trip_page, 2000, NULL);

  event = EV_MOVING;
  fsm.add_transition(&state_trip_page, &state_moving_screen, event, NULL);

  event = EV_STOPPED_MOVING;
  fsm.add_transition(&state_moving_screen, &state_trip_page, event, NULL);

  event = EV_UPDATE_FROM_BOARD;
  fsm.add_transition(&state_trip_page, &state_trip_page, event, NULL);
}
/* ---------------------------------------------- */

void TRIGGER(uint8_t x, char *s)
{
  if (s != NULL)
  {
    Serial.printf("EVENT: %s\n", s);
  }
  fsm.trigger(x);
}

void TRIGGER(uint8_t x)
{
  switch (x)
  {
  case EV_SERVER_CONNECTED:
    Serial.printf("trigger: EV_SERVER_CONNECTED\n");
    break;
  case EV_SERVER_DISCONNECTED:
    Serial.printf("trigger: EV_SERVER_DISCONNECTED\n");
    break;
  case EV_MOVING:
    Serial.printf("trigger: EV_MOVING\n");
    break;
  case EV_STOPPED_MOVING:
    Serial.printf("trigger: EV_STOPPED_MOVING\n");
    break;
  case EV_BUTTON_CLICK:
    Serial.printf("trigger: EV_BUTTON_CLICK\n");
    break;
  default:
    Serial.printf("WARNING: unhandled trigger\n");
  }
  TRIGGER(x, NULL);
}
