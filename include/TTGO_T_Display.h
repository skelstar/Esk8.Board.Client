// ST7789 135 x 240 display with no chip select line
#include <TFT_eSPI.h>
#include <SPI.h>

#define ST7789_DRIVER // Configure all registers

#define TFT_WIDTH 240
#define TFT_HEIGHT 135

#define CGRAM_OFFSET // Library will add offsets required

// SPRITES stuff
// https://github.com/skelstar/esk8Project/blob/5db5722ed70cbee6d732bda6a1be2c75ffc68f05/Controller/Controller.ino

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
// #define TFT_BACKLIGHT_ON HIGH

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

#define FONT_1_HEIGHT   27
#define FONT_2_HEIGHT   31

#define FONT_2_LINE_HEIGHT  35  // at 4 lines in landscape

#define SMOOTH_FONT

// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY 40000000

#define SPI_READ_FREQUENCY 20000000

#define SPI_TOUCH_FREQUENCY 2500000

// #define SUPPORT_TRANSACTIONS

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

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

//--------------------------------------------------------------------------------

void chunky_draw_digit(
    uint8_t digit,
    uint8_t x,
    uint8_t y,
    uint16_t fg_color,
    uint16_t bg_color,
    uint8_t pixel_size = 1)
{

  for (int xx = 0; xx < 3; xx++)
  {
    for (int yy = 0; yy < 5; yy++)
    {
      uint16_t color = FONT_DIGITS_3x5[digit][yy][xx] ? fg_color : bg_color;
      int x1 = x + (xx * pixel_size);
      int y1 = y + (yy * pixel_size);
      tft.fillRect(x1, y1, pixel_size, pixel_size, color);
    }
  }
}
//--------------------------------------------------------------------------------

void chunkyDrawFloat(uint8_t x, uint8_t y, char *number, char *units, uint8_t spacing, uint8_t pixel_size = 1)
{

  int cursor_x = x;
  int number_len = strlen(number);

  for (int i = 0; i < number_len; i++)
  {
    char ch = number[i];
    if (ch >= '0' and ch <= '9')
    {
      chunky_draw_digit(ch - '0', cursor_x, y, TFT_WHITE, TFT_BLACK, pixel_size);
      cursor_x += 3 * pixel_size + spacing;
    }
    else if (ch == '.')
    {
      tft.fillRect(cursor_x, y + 4 * pixel_size, pixel_size, pixel_size, TFT_WHITE);
      cursor_x += pixel_size + spacing;
    }
    else if (ch == '-')
    {
    }
    else if (ch == ' ')
    {
      cursor_x += 3 * pixel_size + spacing;
    }
    else if (ch == '%')
    {
      chunky_draw_digit(9 + 1, cursor_x, y, TFT_WHITE, TFT_BLACK, pixel_size);
      cursor_x += 3 * pixel_size + spacing;
    }
  }
  // units
  if (units != NULL)
  {
    tft.drawString(units, cursor_x + spacing, y + 10);
  }
}

uint8_t get_chunk_digits_width(char *number, uint8_t spacing, uint8_t pixel_size)
{
  uint8_t width = strlen(number);
  return width * (pixel_size * 3) + (spacing * (width - 1));
}

void chunkyDrawFloat(uint8_t datum, char *number, char *units, uint8_t spacing, uint8_t pixel_size = 1)
{
  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t width = get_chunk_digits_width(number, spacing, pixel_size);
  uint8_t height = pixel_size * 5;

  switch (datum)
  {
  case MC_DATUM:
  {
    x = (TFT_WIDTH / 2) - (width / 2);
    y = TFT_HEIGHT / 2 - (height / 2);
    chunkyDrawFloat(x, y, number, units, spacing, pixel_size);
  }
  break;
  case TL_DATUM:
  {
    chunkyDrawFloat(0, 0, number, units, spacing, pixel_size);
  }
  break;
  case BR_DATUM:
  {
    x = TFT_WIDTH - width;
    y = TFT_HEIGHT - height;
    chunkyDrawFloat(x, y, number, units, spacing, pixel_size);
  }
  break;
  default:
    DEBUG("Unhandled datum");
  }
  return;
}

void chunkyDrawFloat(uint8_t datum, uint8_t y, uint8_t num_chars, char *number, uint8_t size)
{
  int spacing = 1;
  int pixel = 1;
  uint8_t x = 0;

  switch (size)
  {
    case 2:
      spacing = 3;
      pixel = 5;
      break;
    default:
      DEBUGVAL("Unhandled font size: ", size);
      break;
  }

  switch (datum)
  {
    case TR_DATUM:
      x = TFT_WIDTH - (num_chars * (pixel*5) + (num_chars-1) * spacing);
      chunkyDrawFloat(x, y, number, "", spacing, pixel);
      break;
    default:
      DEBUGVAL("Unhandled datum: ", datum);
      break;
  }
}

//---------------------------------------------------------------
void display_initialise()
{
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ready", TFT_WIDTH / 2, TFT_HEIGHT / 2, 2);
}
//---------------------------------------------------------------

void lcd_message(uint8_t datum, char *message, uint8_t x, uint8_t y, uint8_t font)
{
  tft.setTextDatum(datum);
  tft.drawString(message, x, y, font);
}

void lcd_message_middle(uint8_t datum, char *message, uint8_t font)
{
  lcd_message(datum, message, TFT_WIDTH/2, TFT_HEIGHT/2, font);
}
//---------------------------------------------------------------

void lcd_bottom_line(char *message)
{
  tft.setTextDatum(BC_DATUM);
  tft.drawString(message, TFT_WIDTH / 2, TFT_HEIGHT, 1);
}
//---------------------------------------------------------------

void tft_util_draw_digit(
    TFT_eSprite *tft,
    uint8_t digit,
    uint8_t x,
    uint8_t y,
    uint16_t fg_color,
    uint16_t bg_color,
    uint8_t pixel_size = 1)
{

  for (int xx = 0; xx < 3; xx++)
  {
    for (int yy = 0; yy < 5; yy++)
    {
      uint16_t color = FONT_DIGITS_3x5[digit][yy][xx] ? fg_color : bg_color;
      int x1 = x + xx * pixel_size;
      int y1 = y + yy * pixel_size;
      tft->fillRect(x1, y1, pixel_size, pixel_size, color);
    }
  }
}

//---------------------------------------------------------------
#define BATTERY_WIDTH     200
#define BATTERY_HEIGHT    100
#define BORDER_SIZE 12
#define KNOB_HEIGHT 40

void drawBattery(int percent)
{
  tft.fillScreen(TFT_BLACK);

  int outsideX = (TFT_WIDTH - (BATTERY_WIDTH + BORDER_SIZE)) / 2; // includes batt knob
  int outsideY = (TFT_HEIGHT - BATTERY_HEIGHT) / 2;
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
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------