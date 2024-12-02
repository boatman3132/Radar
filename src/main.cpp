#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Button2.h>   // Button handling library

// 包含頁面檔案（假設它們各自定義了不同的頁面顯示函數）
#include "page1.h"
#include "page2.h"
#include "page3.h"
// #include "page4.h"
// #include "page5.h"

#define PIN_POWER_ON 15 // LCD and battery Power Enable
#define PIN_LCD_BL 38 // BackLight enable pin (see Dimming.txt)


// 初始化 TFT 顯示屏
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);  // Background sprite shared across pages

// 當前顯示的頁面編號，初始為第 1 頁
int currentPage = 2;
// 定義總頁數
const int totalPages = 3;            

// 定義按鈕的 GPIO 針腳
Button2 buttonUp(14);    // 向上切換頁面的按鈕，連接 GPIO14
Button2 buttonDown(0);   // 向下切換頁面的按鈕，連接 GPIO0


// 處理長按按鈕的事件
void handleLongPress(Button2 &btn) {
    // 進入深度休眠模式
    esp_deep_sleep_start();
}

// 顯示指定的頁面
void showPage(int page) {
  // 根據頁面編號顯示對應的內容
  switch (page) {
    case 1: showPage1(tft, background); break;
    case 2: showPage2(tft, background, currentHeartRateBPM, currentBreathingRateRPM); break; // 傳遞心率和呼吸率
    case 3: showPage3(tft, background); break;
    // case 4: showPage4(tft, background); break;
    // case 5: showPage5(tft, background); break;
  }
}

// 處理向上按鈕的事件
void handleUp(Button2 &btn) {
    Serial.println("按鈕已成功按下");
    background.fillSprite(TFT_BLACK);  // 清空螢幕內容
    currentPage--;  // 頁碼減 1
    if (currentPage < 1) currentPage = totalPages;  // 如果小於 1，則跳轉到最後一頁

    if (currentPage == 2) {
        currentX = 10;  // 當進入 page2 時重置 currentX
    }

    showPage(currentPage);  // 顯示新頁面
}

// 處理向下按鈕的事件
void handleDown(Button2 &btn) {
    Serial.println("按鈕已成功按下");

    background.fillSprite(TFT_BLACK);  // 清空螢幕內容
    currentPage++;  // 頁碼加 1
    if (currentPage > totalPages) currentPage = 1;  // 如果大於總頁數，則跳轉到第一頁

    if (currentPage == 2) {
        currentX = 10;  // 當進入 page2 時重置 currentX
    }

    showPage(currentPage);  // 顯示新頁面
}


void setup() {


  Serial.begin(9600);  // 初始化 Serial 通訊，波特率設置為 9600


  pinMode(PIN_POWER_ON, OUTPUT); //triggers the LCD backlight
  pinMode(PIN_LCD_BL, OUTPUT); // BackLight enable pin

  digitalWrite(PIN_POWER_ON, HIGH);
  digitalWrite(PIN_LCD_BL, HIGH);
  // 初始化 TFT 顯示屏
  tft.init();
  tft.setRotation(3);  // 設定顯示旋轉角度
  // 建立背景精靈並填充黑色
  background.createSprite(tft.width(), tft.height());
  background.fillSprite(TFT_BLACK);

  background.setSwapBytes(false);  // 設置字節順序以確保顏色顯示正確

  
    // 配置按鈕事件處理器
    buttonUp.setPressedHandler(handleUp);      // 當按下“向上”按鈕時，呼叫 handleUp
    buttonDown.setPressedHandler(handleDown);  // 當按下“向下”按鈕時，呼叫 handleDown

    // 設定長按時間為一秒
    buttonUp.setLongClickTime(1000);  // 設置長按時間為 1000 毫秒

    // 設定長按事件處理器
    buttonUp.setLongClickHandler(handleLongPress);  // 當長按“向上”按鈕時，呼叫 handleLongPress

  // 顯示初始頁面
  showPage(currentPage);
}


void loop() {
  buttonUp.loop();
  buttonDown.loop();

  static unsigned long lastUpdateTime = 0;  // 上次更新的時間
  unsigned long currentTime = millis();     // 當前時間

  // 每隔一秒更新一次心率和呼吸率
  if (currentTime - lastUpdateTime >= 3000) {
    lastUpdateTime = currentTime;  // 更新上次更新的時間


    // 隨機生成心率和呼吸率
    currentHeartRateBPM = random(50, 71);  // 生成 50 到 70 之間的亂數
    currentBreathingRateRPM = random(8, 16);  // 生成 8 到 15 之間的亂數


  }
    // 根據當前頁面編號刷新顯示
  showPage(currentPage);
}