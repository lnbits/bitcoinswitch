#ifdef TFT
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
void setupTFT() {
  tft.init();
  Serial.println("TFT: " + String(TFT_WIDTH) + "x" + String(TFT_HEIGHT));
  Serial.println("TFT pin MISO: " + String(TFT_MISO));
  Serial.println("TFT pin CS: " + String(TFT_CS));
  Serial.println("TFT pin MOSI: " + String(TFT_MOSI));
  Serial.println("TFT pin SCLK: " + String(TFT_SCLK));
  Serial.println("TFT pin DC: " + String(TFT_DC));
  Serial.println("TFT pin RST: " + String(TFT_RST));
  Serial.println("TFT pin BL: " + String(TFT_BL));
  tft.setRotation(1);
  tft.invertDisplay(true);
  tft.fillScreen(TFT_BLACK);
}
void printTFT(String message, int x, int y) {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(x, y);
  tft.println(message);
}
void printHome(bool wifi, bool ws, bool ping) {
  if (ping) {
    tft.fillScreen(TFT_BLUE);
  } else {
    tft.fillScreen(TFT_BLACK);
  }
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(21, 21);
  tft.println("BitcoinSwitch");
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  if (wifi) {
    tft.setCursor(21, 69);
    tft.println("WiFi connected!");
  } else {
    tft.setCursor(21, 69);
    tft.setTextColor(TFT_RED);
    tft.println("No WiFi!");
  }
  tft.setCursor(21, 95);
  if (ws) {
    if (ping) {
      tft.setTextColor(TFT_WHITE);
      tft.println("WS ping!");
    } else {
      tft.setTextColor(TFT_GREEN);
      tft.println("WS connected!");
    }
  }
  else {
    tft.setTextColor(TFT_RED);
    tft.println("No WS!");
  }
}
void clearTFT() {
  tft.fillScreen(TFT_BLACK);
}
void flashTFT() {
  tft.fillScreen(TFT_GREEN);
}
#else
void printTFT(String message, int x, int y) {}
void printHome(bool wifi, bool ws, bool ping) {}
void clearTFT() {}
void flashTFT() {}
#endif
