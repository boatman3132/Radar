#ifndef INITIAL_H
#define INITIAL_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include <WebServer.h>

// 外部變數宣告
extern TFT_eSPI tft;
extern TFT_eSprite background;
extern int currentPage;
extern const int totalPages;
extern float currentHeartRateBPM;
extern float currentBreathingRateRPM;
extern Button2 buttonUp;
extern Button2 buttonDown;
extern WebServer server;

// 函數宣告
void initSystem();
void handleUp(Button2 &btn);
void handleDown(Button2 &btn);
void handleLongPress(Button2 &btn);
void showPage(int page);

#endif