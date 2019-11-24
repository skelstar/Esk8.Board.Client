#ifndef u8g2Lib
#include <U8g2lib.h>
#endif

//--------------------------------------------------------------------------------
void lcdMissingPacketReport(int numPackets)
{
  u8g2.clearBuffer();
  char primaryLine[5];

  itoa(numPackets, primaryLine, 10);

  uint8_t pixelSize = 6;
  uint8_t spacing = 4;
  // int width = strlen(primaryLine) * 3 + (strlen(primaryLine) * (spacing-1));
  chunkyDrawFloat(0, 64/2 - (pixelSize*5)/2, primaryLine, "n", spacing, pixelSize);

  u8g2.sendBuffer();
}