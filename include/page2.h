#ifndef PAGE2_H
#define PAGE2_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// 聲明心跳速率和呼吸速率的變數
extern float currentHeartRateBPM;
extern float currentBreathingRateRPM;

// 聲明存儲心跳和呼吸波形數據的陣列
extern float heartData[200];
extern float breathData[200];

// 聲明呼吸與心跳的相位變數
extern float breathPhase;
extern float heartPhase;

// 聲明當前 x 座標位置
extern int currentX;
// 聲明 x 軸的最大範圍
extern int max_X;

// 聲明函數
void updateHeartAndBreathDisplay(TFT_eSPI &tft, TFT_eSprite &background);
float generatePQRSTWave(float phase);
void showPage2(TFT_eSPI &tft, TFT_eSprite &background, float heartRate, float breathingRate);

#endif // PAGE2_H