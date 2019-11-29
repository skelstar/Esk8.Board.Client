
enum EventsEnum
{
  EV_BUTTON_CLICK,
  SERVER_CONNECTED,
  SERVER_DISCONNECTED,
  MOVING,
  STOPPED_MOVING,
  EV_HELD_DOWN_WAIT,
  EV_NO_HELD_OPTION_SELECTED,
  EV_HELD_OPTION_SELECTED,
  EV_RECV_PACKET,
} event;

void triggerEvent(EventsEnum event);

//-------------------------------
State state_connecting(
  [] { 
    DEBUG("state_connecting");
    lcdConnectingPage("connecting...", vescdata.ampHours, vescdata.odometer);
  }, 
  NULL, 
  NULL
);
//-------------------------------
State state_connected(
  [] { DEBUG("state_connected"); },
  [] { drawBattery(getBatteryPercentage(vescdata.batteryVoltage), changed(CHECK_BATT_VOLTS)); }, 
  NULL
);
//-------------------------------
State state_server_disconnected(
  [] {
    DEBUG("state_server_disconnected");
    lcdConnectingPage("disconnected", vescdata.ampHours, vescdata.odometer); 
  }, 
  NULL, 
  NULL
);
//-------------------------------
State state_battery_voltage_screen(
  [] { 
    DEBUG("state_battery_voltage_screen");
    drawBattery(getBatteryPercentage(vescdata.batteryVoltage), true); 
  },
  [] { drawBattery(getBatteryPercentage(vescdata.batteryVoltage), changed(CHECK_BATT_VOLTS)); },
  NULL
);
//-------------------------------
State state_trip_page(
  [] { 
    DEBUG("state_trip_page");
    lcdTripPage(vescdata.ampHours, vescdata.odometer, vescdata.vescOnline, true); 
  }, 
  [] { 
    lcdTripPage(vescdata.ampHours, vescdata.odometer, vescdata.vescOnline, changed(CHECK_AMP_HOURS)); 
  }, 
  NULL
);
//-------------------------------
State state_moving_screen(
  [] { 
    DEBUG("state_moving_screen");
    clearScreen(); 
  }, 
  NULL, 
  NULL
);
//-------------------------------
State state_button_held_wait(
  [] { 
    DEBUG("state_button_held_wait");
    lcdMessage("...");
  }, 
  NULL, 
  NULL
);
//-------------------------------
State state_button_held_clear_missed_packets(
  [] { 
    DEBUG("state_button_held_clear_missed_packets");
    lcdMessage("Clear missed");
  }, 
  NULL, 
  NULL
);
//-------------------------------
State state_button_held_clear_missed_packets_selected(
  [] { 
    DEBUG("state_button_held_clear_missed_packets");
    lcdMessage("Cleared!");
    missedPacketCounter = 0;
    vescdata.ampHours = 0;
  }, 
  NULL,
  NULL
);
//-------------------------------

Fsm fsm(&state_connecting);

void triggerEvent(EventsEnum event) {
  fsm.trigger(event);
}

void addFsmTransitions() {

  uint8_t event = SERVER_DISCONNECTED;
  fsm.add_transition(&state_battery_voltage_screen, &state_server_disconnected, event, NULL);
  fsm.add_transition(&state_trip_page, &state_server_disconnected, event, NULL);

  event = SERVER_CONNECTED;
  fsm.add_transition(&state_connecting, &state_trip_page, event, NULL);
  // when state connected is entered it will transition to new state after 3 seconds
  // fsm.add_timed_transition(&state_connected, &state_trip_page, 3000, NULL);

  event = EV_RECV_PACKET;
  fsm.add_transition(&state_connecting, &state_trip_page, event, NULL);
  
  event = EV_BUTTON_CLICK;
  fsm.add_transition(&state_trip_page, &state_battery_voltage_screen, event, NULL);
  fsm.add_transition(&state_server_disconnected, &state_battery_voltage_screen, event, NULL);
  // if going to batt screen, go back to trip page after 2 seconds
  fsm.add_timed_transition(&state_battery_voltage_screen, &state_trip_page, 2000, NULL);
  fsm.add_transition(&state_button_held_wait, &state_trip_page, event, NULL);

  event = MOVING;
  fsm.add_transition(&state_trip_page, &state_moving_screen, event, NULL);

  event = STOPPED_MOVING;
  fsm.add_transition(&state_moving_screen, &state_trip_page, event, NULL);

  event = EV_HELD_DOWN_WAIT;
  fsm.add_transition(&state_connecting, &state_button_held_wait, event, NULL);
  fsm.add_transition(&state_server_disconnected, &state_button_held_wait, event, NULL);
  fsm.add_transition(&state_battery_voltage_screen, &state_button_held_wait, event, NULL);
  fsm.add_transition(&state_trip_page, &state_button_held_wait, event, NULL);
  fsm.add_transition(&state_button_held_wait, &state_button_held_clear_missed_packets, event, NULL);


  event = EV_HELD_OPTION_SELECTED;
  fsm.add_transition(&state_button_held_clear_missed_packets, &state_button_held_clear_missed_packets_selected, event, NULL);
  fsm.add_timed_transition(&state_button_held_clear_missed_packets_selected, &state_trip_page, 1000, NULL);

  event = EV_NO_HELD_OPTION_SELECTED;  // no option selected
  fsm.add_transition(&state_button_held_wait, &state_trip_page, event, NULL);
}
/* ---------------------------------------------- */