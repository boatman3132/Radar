#include "initial.h"
#include "website.h"
#include "page1.h"
#include "page2.h"
// #include "page3.h"

#define PIN_POWER_ON 15
#define PIN_LCD_BL 38

// 全域變數定義
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);
int currentPage = 2;
const int totalPages = 2;
float currentHeartRateBPM = 0.0;
float currentBreathingRateRPM = 0.0;
Button2 buttonUp(14);
Button2 buttonDown(0);
WebServer server(80);


void handleLongPress(Button2 &btn) {
    esp_deep_sleep_start();
}

void handleUp(Button2 &btn) {
    background.fillSprite(TFT_BLACK);
    currentPage--;
    if (currentPage < 1) currentPage = totalPages;
    if (currentPage == 2) {
        currentX = 10;
    }
    showPage(currentPage);
}

void handleDown(Button2 &btn) {
    background.fillSprite(TFT_BLACK);
    currentPage++;
    if (currentPage > totalPages) currentPage = 1;
    if (currentPage == 2) {
        currentX = 10;
    }
    showPage(currentPage);
}

void showPage(int page) {
    switch (page) {
        case 1: showPage1(tft, background); break;
        case 2: showPage2(tft, background, currentHeartRateBPM, currentBreathingRateRPM); break;
        // case 3: showPage3(tft, background); break;
    }
}

void initSystem() {
    // 硬體初始化
    pinMode(PIN_POWER_ON, OUTPUT);
    pinMode(PIN_LCD_BL, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    digitalWrite(PIN_LCD_BL, HIGH);
    Serial.begin(9600);
    
    // 顯示器初始化
    tft.init();
    tft.setRotation(3);
    background.createSprite(tft.width(), tft.height());
    background.fillSprite(TFT_BLACK);
    background.setSwapBytes(false);
    
    // 按鈕初始化
    buttonUp.setPressedHandler(handleUp);
    buttonDown.setPressedHandler(handleDown);
    buttonUp.setLongClickTime(1000);
    buttonUp.setLongClickHandler(handleLongPress);
    buttonDown.setLongClickHandler(handleLongPress);
    
    // WiFi 初始化
    const char* ssid = "diningroom";
    const char* password = "thisissamuelh";
    
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    // 等待 WiFi 連接
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi 已連接");
        Serial.print("IP 位址: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi 連接失敗");
    }
    
    // 設定網頁伺服器
    setupServer(server);
    server.begin();
    
    // 顯示初始頁面
    showPage(currentPage);
}