#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define REPORTING_PERIOD_MS 1000
#define MAX_MEASUREMENTS 20
#define MIN_BPM 40
#define MAX_BPM 200

const char* ssid = "";
const char* password = "";
const char* serverUrl = "";

PulseOximeter pox;
uint32_t tsLastReport = 0;

float bpmValues[MAX_MEASUREMENTS];
int bpmCount = 0;

void onBeatDetected() {
  Serial.println("Kalp atışı algılandı!");
}

void connectToWiFi() {
  Serial.print("Wi-Fi'ye bağlanılıyor: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    attempts++;
    if (attempts >= 20) {
      Serial.println("Wi-Fi bağlantısı başarısız.");
      return;
    }
  }

  Serial.println("Wi-Fi bağlantısı başarılı!");
  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());
}

void sendDataToServer(float averageBpm) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"bpm\": " + String(averageBpm, 2) + "}";
    Serial.print("Sunucuya gönderilen veri: ");
    Serial.println(jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Sunucu yanıt kodu: ");
      Serial.println(httpResponseCode);
      Serial.print("Sunucu yanıtı: ");
      Serial.println(response);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (error) {
        Serial.print("JSON parse hatası: ");
        Serial.println(error.f_str());
      } else {
        const char* duygu = doc["duygu"];
        Serial.print("Duygu: ");
        Serial.println(duygu);

        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Duygu:");
        display.setTextSize(2);
        display.setCursor(0, 20);
        display.println(duygu);
        display.setTextSize(1);
        display.setCursor(0, 40);
        display.println("BPM:");
        display.setTextSize(2);
        display.setCursor(0, 50);
        display.println(averageBpm);
        display.display();
      }
    } else {
      Serial.print("Sunucuya veri gönderme hatası. Kod: ");
      Serial.println(httpResponseCode);
      Serial.printf("Hata: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("Wi-Fi bağlantısı yok. Veri gönderilemiyor.");
  }
}

void initializeSensor() {
  Serial.print("MAX30100 Sensörü başlatılıyor... ");
  if (!pox.begin()) {
    Serial.println("Başlatılamadı!");
    while (1) delay(100);
  } else {
    Serial.println("Başarılı!");
    pox.setIRLedCurrent(MAX30100_LED_CURR_14_2MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }
}

void setup() {
  Wire.begin(21, 22);
  Wire.setClock(100000);

  Serial.begin(115200);
  connectToWiFi();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED ekran başlatılamadı.");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hazir...");
  display.display();
  delay(1000);

  initializeSensor();
}

void loop() {
  pox.update();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float bpm = pox.getHeartRate();

    Serial.print("BPM: ");
    Serial.println(bpm);

    if (bpm >= MIN_BPM && bpm <= MAX_BPM && bpmCount < MAX_MEASUREMENTS) {
      bpmValues[bpmCount++] = bpm;
      Serial.print("Kaydedildi [");
      Serial.print(bpmCount);
      Serial.print("/");
      Serial.print(MAX_MEASUREMENTS);
      Serial.println("]");

      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(4);
      display.print(bpmCount);
      display.display();
    } else if (bpmCount < MAX_MEASUREMENTS) {
      Serial.println("Geçersiz BPM değeri.");
    }

    if (bpmCount == MAX_MEASUREMENTS) {
      Serial.println("20 BPM ölçümü tamamlandı:");
      float sum = 0;
      for (int i = 0; i < MAX_MEASUREMENTS; i++) {
        Serial.print("BPM ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(bpmValues[i]);
        sum += bpmValues[i];
      }

      float average = sum / MAX_MEASUREMENTS;
      Serial.print("Ortalama BPM: ");
      Serial.println(average, 2);

      sendDataToServer(average);

      Serial.println("Sensör yeniden başlatılıyor...");
      initializeSensor();

      bpmCount = 0;
      memset(bpmValues, 0, sizeof(bpmValues));
      Serial.println("Ölçüm sıfırlandı.");
    }

    tsLastReport = millis();
  }
}
