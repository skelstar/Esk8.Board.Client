
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
  fsm.add_transition(&state_connecting, &state_trip_page, event, NULL);
  // when state connected is entered it will transition to new state after 3 seconds
  // fsm.add_timed_transition(&state_connected, &state_trip_page, 3000, NULL);

  event = BUTTON_CLICK;
  fsm.add_transition(&state_trip_page, &state_battery_voltage_screen, event, NULL);
  // if going to batt screen, go back to trip page after 2 seconds
  fsm.add_timed_transition(&state_battery_voltage_screen, &state_trip_page, 2000, NULL);
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