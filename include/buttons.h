
LogansGreatButton button0(BUTTON_0_PIN);

void onButtonActionPressed()
{
}

void onButtonPressShortRelease()
{
  TRIGGER(EV_BUTTON_CLICK);
}

void onButtonPressLongStart()
{
}

void onButtonPressLongRelease()
{
}

void onButtonHoldStart()
{
}

void onButtonHoldContinuous()
{
  lcd_message_middle(MC_DATUM, "reseting", 2);
  while(1);
  
  // int secondsPassed = (millis() - button0.getLastPressTime()) / 1000;
  // const int menuOption1Time = 2;

  // switch (secondsPassed)
  // {
  //   case menuOption1Time:
  //     break;
  //   default:
  //     break;
  // }
}

void onButtonHoldRelease()
{
  int secondsPassed = (millis() - button0.getLastPressTime()) / 1000;
  const int menuOption1Time = 2;

  switch (secondsPassed)
  {
  case menuOption1Time:
    break;
  default:
    break;
  }
}

void button0_initialise()
{
  button0.onPressShortRelease(onButtonPressShortRelease);
  button0.onPressLongStart(onButtonPressLongStart);
  button0.onPressLongRelease(onButtonPressLongRelease);
  button0.onHoldStart(onButtonHoldStart);
  button0.onHoldContinuous(onButtonHoldContinuous);
  button0.onHoldRelease(onButtonHoldRelease);
}