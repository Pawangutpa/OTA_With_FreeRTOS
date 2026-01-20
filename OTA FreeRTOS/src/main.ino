r9wc6ay9#include <WiFi.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>

/* ------------ WIFI CONFIG ------------ */
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

/* ------------ TASK HANDLES ------------ */
TaskHandle_t otaTaskHandle;
TaskHandle_t ledTaskHandle;

/* ------------ OTA TASK ------------ */
void otaTask(void *pvParameters) {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("esp32-ota")) {
    Serial.println("Error starting mDNS");
    while (1) delay(1000);
  }

  ArduinoOTA.setHostname("esp32-ota");

  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start Updating...");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
    Serial.flush();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");

  while (true) {
    ArduinoOTA.handle();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

/* ------------ LED BLINK TASK ------------ */
void ledTask(void *pvParameters) {
  pinMode(2, OUTPUT);

  while (true) {
    digitalWrite(2, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    digitalWrite(2, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

/* ------------ SETUP ------------ */
void setup() {
  Serial.begin(115200);

  xTaskCreate(otaTask, "OTA_Task", 8192, NULL, 1, &otaTaskHandle);
  xTaskCreate(ledTask, "LED_Task", 2048, NULL, 1, &ledTaskHandle);
}

/* ------------ LOOP ------------ */
void loop() {
  // empty (FreeRTOS running)
}
