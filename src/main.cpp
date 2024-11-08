#include <TFT_eSPI.h>
#include <deque>
#include <numeric>

TFT_eSPI tft = TFT_eSPI(); // 初始化顯示器
unsigned long lastUpdateTime = 0;
unsigned long lastRandomUpdateTime = 0;

float heartData[320] = {0};  // 擴大數據數組，以適應螢幕寬度
float breathData[320] = {0};

float currentHeartRateBPM = 70; // 初始心率
float currentBreathingRateRPM = 12; // 初始呼吸速率
std::deque<float> breathRateHistory;
float smoothBreathRate = currentBreathingRateRPM;
float breathPhase = 0;
float heartPhase = 0;

int currentX = 10;  // 起始繪製位置
int lastHeartRate = -1; // 上次顯示的心率數據
int lastBreathingRate = -1; // 上次顯示的呼吸數據

void setup() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  randomSeed(analogRead(0));
}

float generatePQRSTWave(float phase) {
  // P 波：通常是小幅正弦波，表示心房去極化
  if (phase < 0.6 * PI) {           
    return 0.5 * sin(20 * phase);  // 小幅度的正弦波

  // Q 波：負向尖峰，比較小
  } else if (phase < 0.75 * PI) {     
    return -0.5;  // 短暫負向偏移

  // R 波：正向尖峰，幅度較大
  } else if (phase < 0.9 * PI) {     
    return 3.0;  // 大幅度正向尖峰

  // S 波：負向尖峰，比 Q 波略大，但較 R 波小
  } else if (phase < 1.05 * PI) {     
    return -1.0;  // 負向尖峰

  // T 波：較緩和的正弦波，表示心室去極化
  } else if (phase < PI) {           
    return 1.0 * sin(2 * (phase - 1.1 * PI));  // 緩和的正向波動

  // 返回到基線
  } else {                           
    return 0;
  }
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

  // 平滑呼吸速率
  float smoothFactor = 0.1;
  smoothBreathRate = smoothFactor * currentBreathingRateRPM + (1 - smoothFactor) * smoothBreathRate;
  breathRateHistory.push_back(smoothBreathRate);
  if (breathRateHistory.size() > 150) {
    breathRateHistory.pop_front();
  }
  float avgBreathingRate = std::accumulate(breathRateHistory.begin(), breathRateHistory.end(), 0.0) / breathRateHistory.size();

  // 如果心率或呼吸速率改變，則更新顯示
  if ((int)currentHeartRateBPM != lastHeartRate) {
    tft.fillRect(10, 10, 300, 20, TFT_BLACK); // 擴大清除區域，以適應更大字體

    // 設定心率文字和數值顯示
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);  // 設定統一的字體大小

    // 顯示 "Heart Rate: " 和數值
    tft.drawString("Heart Rate: " + String(currentHeartRateBPM, 0) + " BPM", 10, 10);

    lastHeartRate = currentHeartRateBPM;
  }

  if ((int)avgBreathingRate != lastBreathingRate) {
    tft.fillRect(10, 30, 300, 20, TFT_BLACK); // 擴大清除區域，以適應更大字體

    // 設定呼吸速率文字和數值顯示
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);  // 設定統一的字體大小

    // 顯示 "Breathing Rate: " 和數值
    tft.drawString("Breathing Rate: " + String(avgBreathingRate, 0) + " RPM", 10, 30);

    lastBreathingRate = avgBreathingRate;
  }

  // 計算呼吸相位
  float omegaBreath = 2 * PI * avgBreathingRate / 60;
  breathPhase += omegaBreath * dt;
  if (breathPhase >= 2 * PI) breathPhase -= 2 * PI;
  breathData[currentX - 10] = 20 * sin(breathPhase);

  // 計算心跳相位並產生 PQRST 波形
  float omegaHeart = 2 * PI * currentHeartRateBPM / 60;
  heartPhase += omegaHeart * dt;
  if (heartPhase >= 2 * PI) heartPhase -= 2 * PI;
  heartData[currentX - 10] = generatePQRSTWave(heartPhase);

  int up_y_position = 60;
  int down_y_position = 180;

  // 畫心跳波形
  int y1 = map(heartData[currentX - 10], -2, 3, down_y_position, up_y_position);
  if (currentX > 10) {
    int y2 = map(heartData[currentX - 11], -2, 3, down_y_position, up_y_position);
    tft.drawLine(currentX - 1, y2, currentX, y1, TFT_GREEN);
  }

  // 畫呼吸波形
  y1 = map(breathData[currentX - 10], -50, 50, down_y_position, up_y_position);
  if (currentX > 10) {
    int y2 = map(breathData[currentX - 11], -50, 50, down_y_position, up_y_position);
    tft.drawLine(currentX - 1, y2, currentX, y1, TFT_YELLOW);
  }

  // 移動到下一個位置
  currentX += 1;

  // 如果到達螢幕最右邊，重置並清除波形區域
  if (currentX >= 320) {
    tft.fillRect(10, up_y_position, 310, down_y_position, TFT_BLACK); // 清除波形區域
    currentX = 10;  // 回到最左邊重新開始繪製
  }

  delay(20); // 控制更新速率
}

