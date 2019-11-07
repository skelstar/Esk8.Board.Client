
LogansGreatButton button(BUTTON_PIN);

void onButtonActionPressed()
{
  Serial.println();
  Serial.println("The button has been PRESSED. What will the future hold?");
}

void onButtonPressShortRelease()
{
  Serial.println("End  of a SHORT Press");
}

void onButtonPressLongStart()
{
}

void onButtonPressLongRelease()
{
}

void onButtonHoldStart()
{
  fsm.trigger(EV_HELD_DOWN_WAIT);
}

void onButtonHoldContinuous()
{
  int secondsPassed = (millis() - button.getLastPressTime()) / 1000;
  const int menuOption1Time = 2;

  switch (secondsPassed)
  {
    case menuOption1Time:
      break;
    default:
      fsm.trigger(EV_HELD_DOWN_WAIT);
      break;
  }
}

void onButtonHoldRelease()
{
  int secondsPassed = (millis() - button.getLastPressTime()) / 1000;
  const int menuOption1Time = 2;

  switch (secondsPassed)
  {
  case menuOption1Time:
    break;
  default:
    fsm.trigger(EV_NO_HELD_OPTION_SELECTED);
    break;
  }
}