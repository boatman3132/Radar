#include <TFT_eSPI.h>
#include <deque>
#include <numeric>
#include <Arduino.h>
#include <stdint.h>

// 定義硬件串口引腳和波特率
#define RX_PIN 17  // 定義接收引腳
#define TX_PIN 18  // 定義發送引腳
#define BAUD_RATE 38400
#define BUFFER_SIZE 192

// 定義magicword
const uint8_t magicword[] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};  // Magicword為8字節
#define MAGICWORD_SIZE 8

// 數據緩衝區
uint8_t buffer[BUFFER_SIZE];
uint8_t bufferIndex = 0;  // 更改變量名稱以避免衝突
uint8_t magicword_index = 0;  // 追蹤magicword匹配進度
bool magicword_detected = false;  // 用於標記是否檢測到magicword

float currentHeartRateBPM = 70.0; // 初始心率
float currentBreathingRateRPM = 12.0; // 初始呼吸速率

// 解析數據幀，提取心跳率和呼吸率
void parseFrame(uint8_t *data, uint16_t length) {
  float heartRate = *(float*)(data -8 + 88);
  float breathingRate = *(float*)(data -8 + 100);

  // 新增判斷條件
  if (heartRate < 0 || heartRate > 300) {
    Serial.println("心跳率數據無效，已捨棄");
    return;
  }
  if (breathingRate < 0 || breathingRate > 100) {
    Serial.println("呼吸率數據無效，已捨棄");
    return;
  }

  currentHeartRateBPM = static_cast<int>(heartRate);
  currentBreathingRateRPM = static_cast<int>(breathingRate);
  
  
  int heartWaveEnergy = static_cast<int>(*(float*)(data -8 + 128));
  int motionDetectionFlag = static_cast<int>(*(float*)(data -8 + 132));

  Serial.print("心跳率 (峰值計數濾波): "); Serial.print(currentHeartRateBPM); Serial.print(" ");
  Serial.print("呼吸率 (峰值計數): "); Serial.print(currentBreathingRateRPM); Serial.print(" ");
  Serial.print("心跳波形能量總和: "); Serial.print(heartWaveEnergy); Serial.print(" ");
  Serial.print("運動檢測標誌: "); Serial.print(motionDetectionFlag); Serial.print(" ");

  Serial.println();
}

TFT_eSPI tft = TFT_eSPI(); // 初始化顯示器
unsigned long lastUpdateTime = 0;
unsigned long lastRandomUpdateTime = 0;

float heartData[320] = {0};  // 擴大數據數組，以適應螢幕寬度
float breathData[320] = {0};

std::deque<float> breathRateHistory;
float smoothBreathRate = currentBreathingRateRPM;
float breathPhase = 0;
float heartPhase = 0;

int currentX = 10;  // 起始繪製位置
int lastHeartRate = -1; // 上次顯示的心率數據
int lastBreathingRate = -1; // 上次顯示的呼吸數據

void setup() {
  // 初始化串口通信
  Serial.begin(BAUD_RATE);  // 與Arduino IDE通信的串口
  Serial1.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN); // 使用Serial1與雷達模塊通信

  // 等待串口初始化
  while (!Serial) {
    ;  // 等待串口初始化完成
  }
  delay(3000);  // 等待一秒

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
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
   if (Serial1.available()) {
       uint8_t byte = Serial1.read();
       if (!magicword_detected) {
           if (byte == magicword[magicword_index]) {
               magicword_index++;
               if (magicword_index == MAGICWORD_SIZE) {
                   magicword_detected = true;
                   magicword_index = 0;
                   bufferIndex = 0;
               }
           } else {
               magicword_index = 0;
           }
       } else {
           buffer[bufferIndex++] = byte;
           if (bufferIndex >= BUFFER_SIZE) {
               parseFrame(buffer, BUFFER_SIZE);
               magicword_detected = false;
               bufferIndex = 0;
           }
       }
   }

   unsigned long now = millis();


   float smoothFactor = 0.1;
   smoothBreathRate = smoothFactor * currentBreathingRateRPM + (1 - smoothFactor) * smoothBreathRate;
   breathRateHistory.push_back(smoothBreathRate);
   if (breathRateHistory.size() > 150) {
       breathRateHistory.pop_front();
   }
   float avgBreathingRate = std::accumulate(breathRateHistory.begin(), breathRateHistory.end(), 0.0) / breathRateHistory.size();

   if ((int)currentHeartRateBPM != lastHeartRate) {
       tft.fillRect(10, 10, 300, 20, TFT_BLACK);
       tft.setTextColor(TFT_GREEN, TFT_BLACK);
       tft.setTextSize(2);
       tft.drawString("Heart Rate: " + String(currentHeartRateBPM, 0) + " BPM", 10, 10);
       lastHeartRate = currentHeartRateBPM;
   }

   if ((int)avgBreathingRate != lastBreathingRate) {
       tft.fillRect(10, 30, 300, 20, TFT_BLACK);
       tft.setTextColor(TFT_YELLOW, TFT_BLACK);
       tft.setTextSize(2);
       tft.drawString("Breathing Rate: " + String(avgBreathingRate, 0) + " RPM", 10, 30);
       lastBreathingRate = avgBreathingRate;
   }

   static unsigned long lastFrameTime = 0;
   if (now - lastFrameTime >= 30) {
       lastFrameTime = now;
       float dt = 0.02; // 固定的步長
       float omegaBreath = 2 * PI * avgBreathingRate / 60;
       breathPhase += omegaBreath * dt * 1.5; 
       if (breathPhase >= 2 * PI) breathPhase -= 2 * PI;
       breathData[currentX - 10] = 20 * sin(breathPhase);

       float omegaHeart = 2 * PI * currentHeartRateBPM / 60;
       heartPhase += omegaHeart * dt * 1.5;
       if (heartPhase >= 2 * PI) heartPhase -= 2 * PI;
       heartData[currentX - 10] = generatePQRSTWave(heartPhase);

       int up_y_position = 60;
       int down_y_position = 165;

       int y1 = map(heartData[currentX - 10], -2, 3, down_y_position, up_y_position);
       if (currentX > 10) {
           int y2 = map(heartData[currentX - 11], -2, 3, down_y_position, up_y_position);
           tft.drawLine(currentX - 1, y2, currentX, y1, TFT_GREEN);
       }

       y1 = map(breathData[currentX - 10], -50, 50, down_y_position, up_y_position);
       if (currentX > 10) {
           int y2 = map(breathData[currentX - 11], -50, 50, down_y_position, up_y_position);
           tft.drawLine(currentX - 1, y2, currentX, y1, TFT_YELLOW);
       }

       currentX += 1;
       if (currentX >= 320) {
           tft.fillRect(10, up_y_position, 310, down_y_position, TFT_BLACK);
           currentX = 10;
       }
   }
}