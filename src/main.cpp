#include <TFT_eSPI.h>
#include <deque>
#include <numeric>

TFT_eSPI tft = TFT_eSPI(); // 初始化顯示器
unsigned long lastUpdateTime = 0;
unsigned long lastRandomUpdateTime = 0;

float heartData[180] = {0};
float breathData[180] = {0};
int heartIndex = 0;
int breathIndex = 0;

float currentHeartRateBPM = 70; // 初始心率
float currentBreathingRateRPM = 12; // 初始呼吸速率
std::deque<float> breathRateHistory;
float smoothBreathRate = currentBreathingRateRPM;
float breathPhase = 0;
float heartPhase = 0;

void setup() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  randomSeed(analogRead(0));
}

void loop() {
  unsigned long now = millis();
  float dt = (now - lastUpdateTime) / 1000.0;
  lastUpdateTime = now;

  // 每隔3秒隨機更新心率和呼吸速率
  if (now - lastRandomUpdateTime >= 3000) {
    currentHeartRateBPM = random(45, 95);
    currentBreathingRateRPM = random(7, 15);
    lastRandomUpdateTime = now;
  }

  // 更新心跳和呼吸數據
  for (int i = 0; i < 179; i++) {
    heartData[i] = heartData[i + 1];
    breathData[i] = breathData[i + 1];
  }

  // 平滑呼吸速率
  float smoothFactor = 0.1;
  smoothBreathRate = smoothFactor * currentBreathingRateRPM + (1 - smoothFactor) * smoothBreathRate;
  breathRateHistory.push_back(smoothBreathRate);
  if (breathRateHistory.size() > 150) {
    breathRateHistory.pop_front();
  }
  float avgBreathingRate = std::accumulate(breathRateHistory.begin(), breathRateHistory.end(), 0.0) / breathRateHistory.size();

  // 計算呼吸相位
  float omegaBreath = 2 * PI * avgBreathingRate / 60;
  breathPhase += omegaBreath * dt;
  if (breathPhase >= 2 * PI) breathPhase -= 2 * PI;
  breathData[179] = 20 * sin(breathPhase);

  // 計算心跳相位
  float omegaHeart = 2 * PI * currentHeartRateBPM / 60;
  heartPhase += omegaHeart * dt;
  if (heartPhase >= 2 * PI) heartPhase -= 2 * PI;
  heartData[179] = 4 * sin(5 * heartPhase) * exp(-20 * (heartPhase / (2 * PI)));

  // 清除並重繪心跳圖和呼吸圖
  tft.fillScreen(TFT_BLACK);
  
  // 顯示心率數據
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("Heart Rate: " + String(currentHeartRateBPM, 0) + " BPM", 10, 10);

  // 顯示呼吸數據
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Breathing Rate: " + String(avgBreathingRate, 0) + " RPM", 10, 30);

  // 畫心跳圖
  for (int i = 0; i < 179; i++) {
    int y1 = map(heartData[i], -0.5, 1.5, 80, 40);
    int y2 = map(heartData[i + 1], -0.5, 1.5, 80, 40);
    tft.drawLine(i + 10, y1, i + 11, y2, TFT_GREEN);
  }

  // 畫呼吸圖
  for (int i = 0; i < 179; i++) {
    int y1 = map(breathData[i], -50, 50, 160, 120);
    int y2 = map(breathData[i + 1], -50, 50, 160, 120);
    tft.drawLine(i + 10, y1, i + 11, y2, TFT_YELLOW);
  }

  delay(33); // 控制更新速率
}
