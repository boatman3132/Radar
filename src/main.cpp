#include <TFT_eSPI.h> // 引入 TFT_eSPI 程式庫

TFT_eSPI tft = TFT_eSPI(); // 初始化顯示器

void setup() {
  tft.init();
  tft.setRotation(3); // 設定螢幕旋轉方向
  tft.fillScreen(TFT_BLACK); // 填充背景為黑色
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // 設定文字顏色
  tft.drawString("Hello, World", 10, 10, 4); // 顯示文字
}

void loop() {
  // 空的 loop，因為我們只需要顯示文字
}
