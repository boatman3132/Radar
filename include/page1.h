#ifndef PAGE1_H
#define PAGE1_H
#include <WiFi.h>
#include <TFT_eSPI.h>

// 變數定義：控制「hello」文字的水平滾動
int scrollX = 0;  // 記錄「hello」文字的 X 座標
const int scrollSpeed = 2;  // 控制滾動速度

// 定義頁面 1 的顯示函數
void showPage1(TFT_eSPI &tft, TFT_eSprite &background) {
    // 清除背景
    background.fillSprite(TFT_BLACK);  // 將背景填充為黑色

    // 設置文字屬性
    background.setTextColor(TFT_WHITE, TFT_BLACK);  // 設置文字顏色為白色，背景顏色為黑色
    background.setTextSize(2);                      // 設置文字大小

    // 顯示靜態 "Page 1: Welcome"
    String ipAddress = WiFi.localIP().toString();
    int x = (tft.width() - background.textWidth("Page 1: Welcome")) / 2;  // 水平居中顯示
    int ipX = (tft.width() - background.textWidth(ipAddress)) / 2;  // 水平居中顯示 IP 地址
    int ipY = tft.height() / 2 + 50;  // 設定 IP 地址顯示的垂直位置
    background.drawString(ipAddress, ipX, ipY);  // 顯示 IP 地址
    int y = tft.height() / 2 - 10;                                        // 垂直位置大約居中
    background.drawString("Page 1: Welcome", x, y);

    // 將背景精靈顯示到顯示屏上
    background.pushSprite(0, 0);
}

#endif // PAGE1_H
