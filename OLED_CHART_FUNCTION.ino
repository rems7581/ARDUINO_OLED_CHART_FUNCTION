/*

OLED CHART
STAVRAKAS - 05/2020

This program allows to plot a value on an oled screen.
The time range is settable with 2 switchs.

Wiring:

OLED:       SWITCH -t   SWITCH +t
GND > GND   D3          D4
Vcc > 3.3V
SCL > A5
SDA > A4


*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
const int OLED_RESET = 4; // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int CHART_OFFX = 24; // Bottom chart offset (pixels)
const int CHART_OFFY = 10; // Left chart offset (pixels)
const int XAXIS_LENGTH = 96; // X axis length (pixels)
const int YAXIS_LENGTH = 40; // Y axis length (pixels)

int tRange = 3; // Initial time range of the chart, 3 multiple

// Timeer
unsigned long time_now = 0;
unsigned long period;
unsigned long t;


// Chart initialisation - Draws axis, and scale values
void oledChartInit(int chartOffx, int chartOffy, int xLength, int yLength, int tRange) {
  period = long(tRange) * 1000 / xLength;
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.setCursor(display.width()/2 - 35, 0);
  display.print("Val = ");
  int y_scale = 0;
  for (int i = 0; i <= 2; i++) {
    display.setCursor(0, display.height() - chartOffy - i * (512 * yLength / 1024) - 5);
    display.print(y_scale);
    y_scale += 512;
  }
  int x_scale = tRange/3;
  for (int i = 1; i <= 3; i++) {
    display.setCursor(chartOffx + i * (tRange/3 * xLength / tRange) - 5, display.height() - 8);
    display.print(x_scale);
    x_scale += tRange/3;
  }
  display.drawLine(chartOffx, display.height() - chartOffy,
                   chartOffx, display.height() - chartOffy - yLength,
                   SSD1306_WHITE);
  display.drawLine(chartOffx, display.height() - chartOffy,
                   chartOffx + xLength, display.height() - chartOffy,
                   SSD1306_WHITE);
  display.display();
}


// Chart update drawing
void oledChartDraw(int val, int chartOffx, int chartOffy, int xLength, int yLength, int tRange) {
  if (millis() >= time_now + period) {
    time_now = millis();
    int val = analogRead(0);
    int val_map = map(val, 0, 1023, 0, yLength);
    // Uncoment to plot curve
    // display.drawPixel(t + chartOffx, display.height() - (val_map + chartOffy), SSD1306_WHITE);
    // Uncomment to plot solid curve
    display.drawLine(t + chartOffx, display.height() - chartOffy,
                     t + chartOffx, display.height() - (val_map + chartOffy),
                     SSD1306_WHITE);
    display.setCursor(display.width()/2, 0);
    display.print(String(val) + "   ");
    display.display();
    t++;
  }
  if (t > xLength) {
    t = 0;
    display.fillRect(chartOffx + 1, display.height() - (chartOffy + yLength), xLength, yLength, SSD1306_BLACK);
    display.display();
  }
}


// Time range change
int tRangeSW() {
  if (digitalRead(3) == LOW && tRange > 3) {
    t = 0;
    tRange -= 3;
    oledChartInit(CHART_OFFX, CHART_OFFY, XAXIS_LENGTH, YAXIS_LENGTH, tRange);
    delay(200);
  }
  if (digitalRead(4) == LOW && tRange < 99) {
    t = 0;
    tRange += 3;
    oledChartInit(CHART_OFFX, CHART_OFFY, XAXIS_LENGTH, YAXIS_LENGTH, tRange);
    delay(200);
  }
  return tRange;
}



void setup() {
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  oledChartInit(CHART_OFFX, CHART_OFFY, XAXIS_LENGTH, YAXIS_LENGTH, tRange);
}



void loop() {
  tRange = tRangeSW(); // Checks if a time range button has been pressed
  int val = analogRead(0);
  oledChartDraw(val, CHART_OFFX, CHART_OFFY, XAXIS_LENGTH, YAXIS_LENGTH, tRange);
}
