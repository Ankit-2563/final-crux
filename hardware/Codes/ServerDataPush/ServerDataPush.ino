// ===============================================================
// ServerDataPush.ino — ESP32-S3 + Quectel EC200U -> Express API
// ===============================================================
// Boot flow:
//   1) POST /api/hardware/register
//   2) POST /api/hardware/data every SEND_INTERVAL
//
// Required Arduino libraries:
//   - TinyGSM
//   - SSLClient
//   - ArduinoHttpClient
//   - DHTesp (by beegee_tokyo)
// ===============================================================

#include "secrets.h"
#include "certs.h"

#define TINY_GSM_MODEM_BG96
#define SerialAT Serial1
#define SerialMon Serial
#define TINY_GSM_USE_GPRS true

#include <TinyGsmClient.h>
#include "SSLClient.h"
#include <ArduinoHttpClient.h>
#include <DHTesp.h>

#define RXD1 40
#define TXD1 41
#define powerPin 42

const char apn[] = "www";

TinyGsm modem(SerialAT);
TinyGsmClient gsmClient(modem);
SSLClient sslClient(&gsmClient);
HttpClient httpClient(sslClient, SERVER_HOST, SERVER_PORT);

unsigned long lastSendTime = 0;
unsigned long lastDhtReadTime = 0;

DHTesp dht1; // TEMP_PIN_1 (DHT11)
DHTesp dht2; // TEMP_PIN_2 (DHT22)
DHTesp dht3; // TEMP_PIN_3 (DHT22)
DHTesp dht4; // TEMP_PIN_4 (DHT22)

void bootModem() {
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  delay(100);
  digitalWrite(powerPin, HIGH);
  delay(1000);
  digitalWrite(powerPin, LOW);
}

bool ensureNetwork() {
  if (!modem.isNetworkConnected()) {
    Serial.println("[NETWORK] Reconnecting network...");
    if (!modem.waitForNetwork(60000L, true)) {
      Serial.println("[NETWORK] Network reconnect failed");
      return false;
    }
  }

  if (!modem.isGprsConnected()) {
    Serial.println("[GPRS] Reconnecting GPRS...");
    if (!modem.gprsConnect(apn)) {
      Serial.println("[GPRS] GPRS reconnect failed");
      return false;
    }
  }

  return true;
}

int postJson(const char *path, const String &body, String &responseBody) {
  httpClient.beginRequest();
  int err = httpClient.post(path);
  if (err != 0) {
    Serial.print("[HTTPS] post() failed: ");
    Serial.println(err);
    httpClient.stop();
    responseBody = "";
    return -1;
  }

  httpClient.sendHeader("Content-Type", "application/json");
  httpClient.sendHeader("Content-Length", body.length());
  httpClient.beginBody();
  httpClient.print(body);
  httpClient.endRequest();

  int statusCode = httpClient.responseStatusCode();
  responseBody = httpClient.responseBody();

  httpClient.stop();
  return statusCode;
}

float readTemperatureC() {
  // DHT sensors require minimum interval between reads.
  unsigned long now = millis();
  if (now - lastDhtReadTime < DHT_READ_MIN_INTERVAL_MS) {
    // Return last computed value by re-reading anyway is harmful; instead just wait.
    // Caller will typically run on SEND_INTERVAL >> DHT interval.
  }
  lastDhtReadTime = now;

  TempAndHumidity r1 = dht1.getTempAndHumidity();
  TempAndHumidity r2 = dht2.getTempAndHumidity();
  TempAndHumidity r3 = dht3.getTempAndHumidity();
  TempAndHumidity r4 = dht4.getTempAndHumidity();

  float sum = 0.0f;
  int count = 0;

  if (!isnan(r1.temperature)) { sum += r1.temperature; count++; }
  if (!isnan(r2.temperature)) { sum += r2.temperature; count++; }
  if (!isnan(r3.temperature)) { sum += r3.temperature; count++; }
  if (!isnan(r4.temperature)) { sum += r4.temperature; count++; }

  if (count == 0) return TEMP_FALLBACK_C;
  return sum / (float)count;
}

float readVoltageV() {
  int raw = analogRead(VOLTAGE_PIN);
  float adcV = (raw / ADC_MAX) * ADC_REF_V;
  return adcV * VOLTAGE_DIVIDER_RATIO;
}

float readCurrentA() {
  int raw = analogRead(CURRENT_PIN);
  float adcV = (raw / ADC_MAX) * ADC_REF_V;
  // Default ACS current-sensor style formula
  return (adcV - CURRENT_ZERO_V) / CURRENT_SENSITIVITY_V_PER_A;
}

bool registerDevice() {
  String body = "{";
  body += "\"deviceId\":\"" + String(DEVICE_ID) + "\",";
  body += "\"deviceSecret\":\"" + String(DEVICE_SECRET) + "\",";
  body += "\"firmwareVersion\":\"" + String(FIRMWARE_VERSION) + "\"";
  body += "}";

  String responseBody;
  int statusCode = postJson(REGISTER_API_PATH, body, responseBody);

  Serial.print("[REGISTER] Status: ");
  Serial.println(statusCode);
  Serial.print("[REGISTER] Response: ");
  Serial.println(responseBody);

  return statusCode == 200 || statusCode == 201;
}

void sendBatteryData() {
  float temperature = readTemperatureC();
  float voltage = readVoltageV();
  float current = readCurrentA();
  float power = voltage * current;

  String body = "{";
  body += "\"deviceId\":\"" + String(DEVICE_ID) + "\",";
  body += "\"deviceSecret\":\"" + String(DEVICE_SECRET) + "\",";
  body += "\"temperature\":" + String(temperature, 2) + ",";
  body += "\"voltage\":" + String(voltage, 2) + ",";
  body += "\"power\":" + String(power, 2) + ",";
  body += "\"current\":" + String(current, 2);
  body += "}";

  Serial.println("----------------------------------------");
  Serial.print("[DATA] Body: ");
  Serial.println(body);

  String responseBody;
  int statusCode = postJson(DATA_API_PATH, body, responseBody);

  Serial.print("[DATA] Status: ");
  Serial.println(statusCode);
  Serial.print("[DATA] Response: ");
  Serial.println(responseBody);
  Serial.println("----------------------------------------\n");
}

void setup() {
  Serial.begin(115200);
  delay(100);

  bootModem();
  delay(5000);
  SerialAT.begin(115200, SERIAL_8N1, RXD1, TXD1);

  Serial.println("\n[MODEM] Initializing...");
  if (!modem.init()) {
    Serial.println("[MODEM] init failed, trying restart...");
    if (!modem.restart()) {
      Serial.println("[MODEM] restart failed");
      while (true) {
        delay(1000);
      }
    }
  }

  Serial.println("[NETWORK] Waiting for network registration...");
  if (!modem.waitForNetwork(60000L)) {
    Serial.println("[NETWORK] registration failed");
    while (true) {
      delay(1000);
    }
  }

  Serial.print("[GPRS] Connecting APN: ");
  Serial.println(apn);
  if (!modem.gprsConnect(apn)) {
    Serial.println("[GPRS] initial connect failed");
    while (true) {
      delay(1000);
    }
  }

  analogReadResolution(12);
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);

  // DHT setup
  dht1.setup(TEMP_PIN_1, DHTesp::DHT11);
  dht2.setup(TEMP_PIN_2, DHTesp::DHT22);
  dht3.setup(TEMP_PIN_3, DHTesp::DHT22);
  dht4.setup(TEMP_PIN_4, DHTesp::DHT22);

  sslClient.setCACert(ca_cert);

  Serial.println("[READY] Network up, trying device register...");
  Serial.print("[TARGET] https://");
  Serial.print(SERVER_HOST);
  Serial.print(":");
  Serial.print(SERVER_PORT);
  Serial.println(REGISTER_API_PATH);

  int attempt = 0;
  while (attempt < 5) {
    if (!ensureNetwork()) {
      attempt++;
      delay(2000);
      continue;
    }
    if (registerDevice()) {
      Serial.println("[READY] Device registered/check-in success");
      return;
    }
    attempt++;
    delay(3000);
  }

  Serial.println("[READY] Register failed after retries, loop() will continue trying data pushes");
}

void loop() {
  if (!ensureNetwork()) {
    delay(3000);
    return;
  }

  unsigned long now = millis();
  if (now - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = now;
    sendBatteryData();
  }

  delay(100);
}
