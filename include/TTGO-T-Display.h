// ST7789 135 x 240 display with no chip select line

#define ST7789_DRIVER // Configure all registers

#define TFT_WIDTH 135
#define TFT_HEIGHT 240

#define CGRAM_OFFSET // Library will add offsets required

// #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

//#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF

// DSTIKE stepup
//#define TFT_DC    23
//#define TFT_RST   32
//#define TFT_MOSI  26
//#define TFT_SCLK  27

// Generic ESP32 setup
#define TFT_MISO -1
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5 // Not connected
#define TFT_DC 16
#define TFT_RST 23 // Connect reset to ensure display initialises
#define TFT_BL 4
#define TFT_BACKLIGHT_ON HIGH

// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
// #define TFT_CS   -1      // Define as not used
// #define TFT_DC   PIN_D1  // Data Command control pin
//#define TFT_RST  PIN_D4  // TFT reset pin (could connect to NodeMCU RST, see next line)
// #define TFT_RST  -1      // TFT reset pin connect to NodeMCU RST, must also then add 10K pull down to TFT SCK

#define LOAD_GLCD  // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2 // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4 // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6 // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7 // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8 // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY 40000000

#define SPI_READ_FREQUENCY 20000000

#define SPI_TOUCH_FREQUENCY 2500000

// #define SUPPORT_TRANSACTIONS

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#define FONT_SIZE_XSMALL u8g2_font_profont10_tr

#define FONT_SIZE_SMALL u8g2_font_profont12_tr

#define FONT_SIZE_MED_SMALL u8g2_font_profont15_tr
#define FONT_SIZE_MED_SMALL_LINE_HEIGHT 15

#define FONT_SIZE_MED u8g2_font_profont17_tr
#define FONT_SIZE_MED_LINE_HEIGHT 17
#define FONT_SIZE_MED_LINE_1 0
#define FONT_SIZE_MED_LINE_2 FONT_SIZE_MED_LINE_HEIGHT
#define FONT_SIZE_MED_LINE_3 FONT_SIZE_MED_LINE_HEIGHT * 2
#define FONT_SIZE_MED_LINE_4 FONT_SIZE_MED_LINE_HEIGHT * 3

#define BETWEEN_LINE1_AND_LINE2 64 / 4
#define BETWEEN_LINE2_AND_LINE3 (64 / 4) * 2

#define FONT_SIZE_LG u8g2_font_profont29_tr
#define FONT_SIZE_LG_LINE_HEIGHT 29
#define FONT_SIZE_LG_ALL (64 / 2) - FONT_SIZE_LG_LINE_HEIGHT / 2
#define FONT_SIZE_LG_LINE_1 0
#define FONT_SIZE_LG_LINE_2 FONT_SIZE_MED_LINE_HEIGHT

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

// https://github.com/skelstar/esk8Project/blob/master/Controller/Display.h
const bool FONT_DIGITS_3x5[11][5][3] = {
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
    },
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
    },
    {
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1},
    },
    {
        {1, 1, 1},
        {0, 0, 1},
        {0, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
    },
    {
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 0, 1},
        {0, 0, 1},
    },
    {
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
    },
    {
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
    },
    {
        {1, 1, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
    },
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
    },
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
    },
    // % = 10
    {
        {1, 0, 1},
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0},
        {1, 0, 1},
    }};

//--------------------------------------------------------------------------------
void chunky_draw_digit(
    uint8_t digit,
    uint8_t x,
    uint8_t y,
    uint8_t pixelSize = 1)
{

  for (int xx = 0; xx < 3; xx++)
  {
    for (int yy = 0; yy < 5; yy++)
    {
      int x1 = x + xx * pixelSize;
      int y1 = y + yy * pixelSize;
      uint32_t color = FONT_DIGITS_3x5[digit][yy][xx] ? TFT_WHITE : TFT_BLACK;
      tft.fillRect(x1, y1, pixelSize, pixelSize, TFT_WHITE);
    }
  }
}
//--------------------------------------------------------------------------------
void chunkyDrawFloat(uint8_t x, uint8_t y, char *number, char *units, uint8_t spacing, uint8_t pixelSize = 1)
{

  int cursor_x = x;
  int number_len = strlen(number);

  for (int i = 0; i < number_len; i++)
  {
    char ch = number[i];
    if (ch >= '0' and ch <= '9')
    {
      chunky_draw_digit(ch - '0', cursor_x, y, pixelSize);
      cursor_x += 3 * pixelSize + spacing;
    }
    else if (ch == '.')
    {
      tft.fillRect(cursor_x, y + 4 * pixelSize, pixelSize, pixelSize, TFT_WHITE);
      cursor_x += pixelSize + spacing;
    }
    else if (ch == '-')
    {
    }
    else if (ch == ' ')
    {
      cursor_x += 3 * pixelSize + spacing;
    }
    else if (ch == '%')
    {
      chunky_draw_digit(9 + 1, cursor_x, y, pixelSize);
      cursor_x += 3 * pixelSize + spacing;
    }
  }
  // units
  tft.setTextSize(3);
  tft.drawString(units, cursor_x + spacing, y + tft.textWidth(units));
  // u8g2.setFont(FONT_SIZE_MED_SMALL);
  // u8g2.drawStr(cursor_x + spacing, y + u8g2.getMaxCharHeight(), units);
}
//--------------------------------------------------------------------------------
#define BAR_GRAPH_THICKNESS 5
void lcdBarGraph(float percentage)
{
  float x2 = tft.width() * 1.0 * percentage;
  tft.fillRect(0, tft.width() - BAR_GRAPH_THICKNESS, x2, BAR_GRAPH_THICKNESS, TFT_WHITE);
}
//--------------------------------------------------------------------------------
void clearScreen()
{
  tft.fillScreen(TFT_BLACK);  
}
//--------------------------------------------------------------------------------
char *getFloatString(char *buff, float val, uint8_t upper, uint8_t lower)
{
  dtostrf(val, upper, lower, buff);
  return buff;
}
//--------------------------------------------------------------------------------
char *getParamFloatString(char *buff, float val, uint8_t upper, uint8_t lower, char *param)
{
  dtostrf(val, upper, lower, buff);
  sprintf(buff, param, buff);
  return buff;
}
//--------------------------------------------------------------------------------
void lcd_medium_float_text(
    uint8_t x,
    uint8_t y,
    char *label,
    char *paramtext,
    float value)
{
  tft.setTextDatum(TL_DATUM);
  // u8g2.setFontPosTop();
  char buff2[8];
  char buff[8]; // Buffer big enough for 7-character float
  dtostrf(value, 5, 1, buff);
  sprintf(buff2, paramtext, buff);
  tft.setTextSize(4);
  int width = tft.textWidth(buff2);
  tft.drawString(label, x, y);
  tft.drawString(buff2, tft.width()-width, y);
}
//--------------------------------------------------------------------------------
// void lcd_paramText(uint8_t x, uint8_t y, char *paramtext, float val1, float val2)
// {
//   u8g2.setFontPosTop();
//   char buffx[8];
//   char buff1[8]; // Buffer big enough for 7-character float
//   char buff2[8]; // Buffer big enough for 7-character float
//   sprintf(buffx, paramtext, getFloatString(buff1, val1, 4, 0), getFloatString(buff2, val2, 2, 1));
//   u8g2.setFont(FONT_SIZE_MED); // full
//   int width = u8g2.getStrWidth(buffx);
//   u8g2.drawStr(x, y, buffx);
// }
//--------------------------------------------------------------------------------
// void lcd_line_text(uint8_t x, uint8_t y, char *text, bool centered)
// {
//   if (centered)
//   {
//     u8g2.setFontPosCenter();
//   }
//   else
//   {
//     u8g2.setFontPosTop();
//   }
//   u8g2.setFont(FONT_SIZE_MED); // full
//   u8g2.drawStr(x, y, text);
// }
//--------------------------------------------------------------------------------
void lcdPrimaryTemplate(
    char *topLine,
    char *primaryLine,
    char *primaryUnits,
    char *bottomLineLeft,
    char *bottomLineRight,
    bool warning)
{
  tft.fillScreen(TFT_BLACK);

  // top line
  if (warning)
  {
    tft.fillRect(0, 0, tft.width(), FONT_SIZE_MED_SMALL_LINE_HEIGHT - 1, TFT_RED);
  }
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(2);
  int width = tft.textWidth(topLine);
  tft.drawString(topLine, (tft.width()-width)/2, 0);

  // middle line
  uint8_t pixelSize = 6;
  uint8_t spacing = 4;
  width = strlen(primaryLine) * 3 + (strlen(primaryLine) * (spacing - 1));
  chunkyDrawFloat(0 + width / 2, tft.height() / 2 - (pixelSize * 5) / 2, primaryLine, primaryUnits, spacing, pixelSize);
  // bottom line
  tft.setTextDatum(BL_DATUM); 
  tft.setTextSize(2);
  // left
  tft.drawString(bottomLineLeft, 0, tft.height());
  // right
  width = tft.textWidth(bottomLineRight);
  tft.drawString(bottomLineRight, tft.width() - tft.textWidth(bottomLineRight), tft.height());
}
//--------------------------------------------------------------------------------
void lcdConnectingPage(char *message, float ampHours, float odo)
{
  char avgbuff[8];
  char aHbuff[8];
  char kmBuff[8];
  float avgAh = ampHours > 0.0 && odo > 0.0
                    ? ampHours / odo
                    : 0.0;

  lcdPrimaryTemplate(
      "connecting...",
      /*primary*/ getFloatString(avgbuff, avgAh, 3, 0),
      /*primaryunits*/ "mAh/km",
      /*left*/ getParamFloatString(aHbuff, ampHours, 3, 0, "%smAh"),
      /*right*/ getParamFloatString(kmBuff, odo, 2, 1, "%skm"),
      /*warning*/ true);
}
//--------------------------------------------------------------------------------
void lcdTripPage(float ampHours, float odo, bool vescOnline, bool update)
{
  if (!update)
  {
    return;
  }
  tft.fillScreen(TFT_BLACK);
  char avgbuff[8];
  char aHbuff[8];
  char kmBuff[8];
  char titleBuff[16];
  float avgAh = ampHours > 0.0 && odo > 0.0 ? ampHours / odo : 0.0;

  strcpy(titleBuff, vescOnline
                        // 01234567890123456
                        ? "Trip"
                        : "VESC OFFLINE!!!");

  lcdPrimaryTemplate(
      /*title*/ titleBuff,
      /*primary*/ getFloatString(avgbuff, avgAh, 3, 0),
      /*primaryunits*/ "mAh/km",
      /*left*/ getParamFloatString(aHbuff, ampHours, 3, 0, "%smAh"),
      /*right*/ getParamFloatString(kmBuff, odo, 2, 1, "%skm"),
      /*warning*/ vescOnline == false);
}
//--------------------------------------------------------------------------------
void lcdMessage(char *message)
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  int width = tft.textWidth(message);
  tft.drawString(message, tft.width()/2 - width, tft.height()/2);
}

//--------------------------------------------------------------------------------
#define BATTERY_WIDTH 100
#define BATTERY_HEIGHT 50
#define BORDER_SIZE 6
#define KNOB_HEIGHT 20

void drawBattery(int percent, bool update)
{
  if (!update)
  {
    return;
  }

  tft.fillScreen(TFT_BLACK);
  int outsideX = (tft.width() - (BATTERY_WIDTH + BORDER_SIZE)) / 2; // includes batt knob
  int outsideY = (tft.height() - BATTERY_HEIGHT) / 2;
  tft.fillRect(outsideX, outsideY, BATTERY_WIDTH, BATTERY_HEIGHT, TFT_WHITE);
  tft.fillRect(
      outsideX + BATTERY_WIDTH,
      outsideY + (BATTERY_HEIGHT - KNOB_HEIGHT) / 2,
      BORDER_SIZE,
      KNOB_HEIGHT,
      TFT_WHITE); // knob
  tft.fillRect(
      outsideX + BORDER_SIZE,
      outsideY + BORDER_SIZE,
      BATTERY_WIDTH - BORDER_SIZE * 2,
      BATTERY_HEIGHT - BORDER_SIZE * 2,
      TFT_BLACK);
  tft.fillRect(
      outsideX + BORDER_SIZE * 2,
      outsideY + BORDER_SIZE * 2,
      (BATTERY_WIDTH - BORDER_SIZE * 4) * percent / 100,
      BATTERY_HEIGHT - BORDER_SIZE * 4,
      TFT_WHITE);
}
//--------------------------------------------------------------------------------