#ifndef PAGE2_H
#define PAGE2_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// 初始化心跳速率和呼吸速率的變數
float currentHeartRateBPM = 60.0;  // 心率設定為常數 70 BPM
float currentBreathingRateRPM = 12.0;  // 呼吸速率設定為常數 15 RPM

// 定義存儲心跳和呼吸波形數據的陣列，長度為 200
float heartData[200] = {0};
float breathData[200] = {0};

// 呼吸與心跳的相位變數
float breathPhase = 0;
float heartPhase = 0;

// 當前 x 座標位置，用於顯示波形
int currentX = 10;
// x 軸的最大範圍，用於實現滾動顯示效果
int max_X = 200;



// 更新顯示的心率和呼吸速率
void updateHeartAndBreathDisplay(TFT_eSPI &tft, TFT_eSprite &background) {
    // 更新心率顯示
    background.setTextColor(TFT_GREEN, TFT_BLACK);  // 設置文字顏色為綠色，背景為黑色
    background.loadFont("NotoSansBold15");  // 加載字體
    background.fillRect(210, 10, 100, 40, TFT_BLACK);  // 清除心率顯示區域
    background.setCursor(230, 20);  // 設置顯示心率的座標
    background.print(String(currentHeartRateBPM, 0));  // 顯示心率

    // 更新呼吸速率顯示
    background.setTextColor(TFT_YELLOW, TFT_BLACK);  // 設置文字顏色為黃色，背景為黑色
    background.fillRect(210, 70, 100, 40, TFT_BLACK);  // 清除呼吸速率顯示區域
    background.setCursor(230, 80);  // 設置顯示呼吸速率的座標
    background.print(String(currentBreathingRateRPM, 0));  // 顯示呼吸速率
}


// 生成心跳 (PQRST) 波形的函數
float generatePQRSTWave(float phase) {
  if (phase < 0.6 * PI) {
    return 0.5 * sin(20 * phase);  // P 波的初始部分 (小波動)
  } else if (phase < 0.75 * PI) {
    return -0.5;  // Q 波 (下降)
  } else if (phase < 0.9 * PI) {
    return 3.0;   // R 波 (快速上升)
  } else if (phase < 1.05 * PI) {
    return -1.0;  // S 波 (下降)
  } else if (phase < PI) {
    return 1.0 * sin(2 * (phase - 1.1 * PI)); // T 波 (小波動)
  } else {
    return 0;    // 相位結束，返回 0
  }
}

// 用於顯示頁面 2 的函數
void showPage2(TFT_eSPI &tft, TFT_eSprite &background) {
    unsigned long now = millis();  // 獲取當前的毫秒數

    static unsigned long lastFrameTime = 0;
    if (now - lastFrameTime >= 1) {
        lastFrameTime = now;  // 更新上次繪製時間
        float dt = 0.023;  // 時間步長

        // 更新呼吸波形數據
        float omegaBreath = 2 * PI * currentBreathingRateRPM / 60;  // 呼吸速率轉換為徑頻率 (rad/s)
        breathPhase += omegaBreath * dt * 1.5;  // 更新呼吸相位
        if (breathPhase >= 2 * PI) breathPhase -= 2 * PI;  // 保持相位在 0 到 2π 範圍內
        breathData[currentX - 10] = 20 * sin(breathPhase);  // 計算並存儲呼吸波形數據

        // 更新心跳波形數據
        float omegaHeart = 2 * PI * currentHeartRateBPM / 60;  // 心跳速率轉換為徑頻率 (rad/s)
        heartPhase += omegaHeart * dt * 1.5;  // 更新心跳相位
        if (heartPhase >= 2 * PI) heartPhase -= 2 * PI;  // 保持相位在 0 到 2π 範圍內
        heartData[currentX - 10] = generatePQRSTWave(heartPhase);  // 計算並存儲心跳波形數據

        // 清除前一段的波形以實現滾動效果
        int up_y_position = 20;  // 波形顯示的起始 y 座標 (上)
        int down_y_position = 150;  // 波形顯示的終止 y 座標 (下)
        int clearWidth = 25;  // 每次清除的寬度 (像素)

        // 在背景精靈上繪製
        background.fillRect(currentX, up_y_position, clearWidth, down_y_position - up_y_position + 20, TFT_BLACK);

        // 繪製新的心跳和呼吸波形數據
        int y1 = map(heartData[currentX - 10], -2, 3, down_y_position - 18, up_y_position + 10);  // 映射心跳數據到顯示屏上
        if (currentX > 10) {
            int y2 = map(heartData[currentX - 11], -2, 3, down_y_position - 18, up_y_position + 10);  // 獲取前一個 x 的心跳數據
            background.drawLine(currentX - 1, y2, currentX, y1, TFT_GREEN);  // 繪製心跳波形
        }

        y1 = map(breathData[currentX - 10], -50, 50, down_y_position + 10, up_y_position);  // 映射呼吸數據到顯示屏上
        if (currentX > 10) {
            int y2 = map(breathData[currentX - 11], -50, 50, down_y_position + 10, up_y_position);  // 獲取前一個 x 的呼吸數據
            background.drawLine(currentX - 1, y2, currentX, y1, TFT_YELLOW);  // 繪製呼吸波形
        }

        // 更新 x 座標，以實現滾動顯示
        currentX += 1;
        if (currentX >= max_X) {
            currentX = 10;  // 如果 x 超出最大範圍，則重置到起始位置
        }

        // 更新顯示的心率和呼吸速率
        updateHeartAndBreathDisplay(tft, background);

        // 將背景顯示到屏幕上
        background.pushSprite(0, 0);
    }
}


#endif // PAGE2_H
