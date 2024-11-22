#include <Arduino.h>
#include <TFT_eSPI.h>

float currentHeartRateBPM = 70.0; // 心率設定為常數 70
float currentBreathingRateRPM = 15.0; // 呼吸速率設定為常數 15

TFT_eSPI tft = TFT_eSPI();

float heartData[200] = {0};
float breathData[200] = {0};

float breathPhase = 0;
float heartPhase = 0;

int currentX = 10;
int max_X = 200;
// 函數宣告
void updateHeartAndBreathDisplay();

void setup() {
  Serial.begin(115200);
  delay(3000);  // 等待串口初始化

  // 初始化 TFT 顯示屏
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(18);

  // 更新顯示的心率和呼吸速率
  updateHeartAndBreathDisplay();
}

float generatePQRSTWave(float phase) {
  if (phase < 0.6 * PI) {
    return 0.5 * sin(20 * phase);
  } else if (phase < 0.75 * PI) {
    return -0.5;
  } else if (phase < 0.9 * PI) {
    return 3.0;
  } else if (phase < 1.05 * PI) {
    return -1.0;
  } else if (phase < PI) {
    return 1.0 * sin(2 * (phase - 1.1 * PI));
  } else {
    return 0;
  }
}

void loop() {
  unsigned long now = millis();

  // 每隔33毫秒更新一次圖形
  static unsigned long lastFrameTime = 0;
  if (now - lastFrameTime >= 33) {
    lastFrameTime = now;
    float dt = 0.02;

    // 更新呼吸數據
    float omegaBreath = 2 * PI * currentBreathingRateRPM / 60;
    breathPhase += omegaBreath * dt * 1.5;
    if (breathPhase >= 2 * PI) breathPhase -= 2 * PI;
    breathData[currentX - 10] = 20 * sin(breathPhase);

    // 更新心跳數據
    float omegaHeart = 2 * PI * currentHeartRateBPM / 60;
    heartPhase += omegaHeart * dt * 1.5;
    if (heartPhase >= 2 * PI) heartPhase -= 2 * PI;
    heartData[currentX - 10] = generatePQRSTWave(heartPhase);

    // 繪製心跳和呼吸波形
    int up_y_position = 20;
    int down_y_position = 150;

    // 清除前一段的波形以實現滾動效果
    int clearWidth = 25;  // 每次清除的寬度
    tft.fillRect(currentX, up_y_position, clearWidth, down_y_position - up_y_position + 20, TFT_BLACK);

    // 繪製新的心跳和呼吸數據
    int y1 = map(heartData[currentX - 10], -2, 3, down_y_position - 18, up_y_position + 10);
    if (currentX > 10) {
      int y2 = map(heartData[currentX - 11], -2, 3, down_y_position - 18, up_y_position + 10);
      tft.drawLine(currentX - 1, y2, currentX, y1, TFT_GREEN);
    }

    y1 = map(breathData[currentX - 10], -50, 50, down_y_position + 10, up_y_position);
    if (currentX > 10) {
      int y2 = map(breathData[currentX - 11], -50, 50, down_y_position + 10, up_y_position);
      tft.drawLine(currentX - 1, y2, currentX, y1, TFT_YELLOW);
    }

    // 更新X座標，並實現滾動
    currentX += 1;
    if (currentX >= max_X) {
      currentX = 10;
    }
  }
}

void updateHeartAndBreathDisplay() {
  // 更新心率顯示
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.loadFont("NotoSansBold15");
  tft.fillRect(210, 10, 100, 40, TFT_BLACK);
  tft.setCursor(230, 20);
  tft.print(String(currentHeartRateBPM, 0));

  // 更新呼吸速率顯示
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.fillRect(210, 70, 100, 40, TFT_BLACK);
  tft.setCursor(230, 80);
  tft.print(String(currentBreathingRateRPM, 0));
}
