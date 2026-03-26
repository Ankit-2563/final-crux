// ============================================================
// HTTPSTest.ino — ESP32-S3 + Quectel EC200U HTTPS Client
// ============================================================
// Sends a random value (1-10) to the server via HTTPS POST
// every 10 seconds over 4G LTE.
//
// Libraries required (install via Arduino Library Manager):
//   - TinyGSM        (by Volodymyr Shymanskyy)
//   - SSLClient      (by govorox — https://github.com/govorox/SSLClient)
//   - ArduinoHttpClient (by Arduino)
// ============================================================

#include "config.h"
#include "certs.h"

#define TINY_GSM_MODEM_BG96    // Works for Quectel EC200U
#define SerialAT Serial1
#define SerialMon Serial
#define TINY_GSM_USE_GPRS true

#include <TinyGsmClient.h>
#include "SSLClient.h"
#include <ArduinoHttpClient.h>

// Pin definitions (matching VVM601 board)
#define RXD1 40       // ESP32-S3 pin 40 → 4G Module Tx
#define TXD1 41       // ESP32-S3 pin 41 → 4G Module Rx
#define powerPin 42   // ESP32-S3 pin 42 → 4G Module power

// APN — leave empty for auto-detect, or set your carrier's APN
const char apn[] = "";  // e.g. "jionet" for Jio

// Modem and client stack
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient gsmClient(modem);
SSLClient sslClient(&gsmClient);
HttpClient httpClient(sslClient, SERVER_HOST, SERVER_PORT);

// Timing
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Power on the 4G module
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  delay(100);
  digitalWrite(powerPin, HIGH);
  delay(1000);
  digitalWrite(powerPin, LOW);

  Serial.println("\n========================================");
  Serial.println("  VVM601 HTTPS Test — Starting up");
  Serial.println("========================================");

  delay(5000);
  SerialAT.begin(115200, SERIAL_8N1, RXD1, TXD1);

  // Initialize modem
  Serial.println("[MODEM] Initializing...");
  if (!modem.init()) {
    Serial.println("[MODEM] Init failed, retrying with restart...");
    if (!modem.restart()) {
      Serial.println("[MODEM] Restart failed! Check connections.");
      while (true) { delay(1000); }  // Halt
    }
  }

  String modemName = modem.getModemName();
  Serial.print("[MODEM] Name: ");
  Serial.println(modemName);

  String modemInfo = modem.getModemInfo();
  Serial.print("[MODEM] Info: ");
  Serial.println(modemInfo);

  // Wait for network
  Serial.println("[NETWORK] Waiting for network registration...");
  if (!modem.waitForNetwork(60000L)) {
    Serial.println("[NETWORK] Failed to register! Check SIM card.");
    while (true) { delay(1000); }
  }
  Serial.println("[NETWORK] Registered successfully");

  // Connect GPRS
  Serial.print("[GPRS] Connecting to APN: ");
  Serial.println(strlen(apn) > 0 ? apn : "(auto-detect)");
  if (!modem.gprsConnect(apn)) {
    Serial.println("[GPRS] Connection failed!");
    while (true) { delay(1000); }
  }
  Serial.println("[GPRS] Connected — LTE data active");

  // Set the CA certificate for server verification (TLS 1.2)
  sslClient.setCACert(ca_cert);

  Serial.println("\n[READY] Will send HTTPS POST every 10 seconds");
  Serial.print("[SERVER] https://");
  Serial.print(SERVER_HOST);
  Serial.print(":");
  Serial.print(SERVER_PORT);
  Serial.println(API_PATH);
  Serial.println("========================================\n");
}

void loop() {
  // Ensure network connectivity
  if (!modem.isNetworkConnected()) {
    Serial.println("[NETWORK] Disconnected, reconnecting...");
    if (!modem.waitForNetwork(60000L, true)) {
      Serial.println("[NETWORK] Reconnect failed, retrying next loop");
      delay(5000);
      return;
    }
    Serial.println("[NETWORK] Reconnected");
  }

  // Ensure GPRS connectivity
  if (!modem.isGprsConnected()) {
    Serial.println("[GPRS] Disconnected, reconnecting...");
    if (!modem.gprsConnect(apn)) {
      Serial.println("[GPRS] Reconnect failed, retrying next loop");
      delay(5000);
      return;
    }
    Serial.println("[GPRS] Reconnected");
  }

  // Send data at intervals
  unsigned long now = millis();
  if (now - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = now;
    sendRandomData();
  }

  delay(100);
}

void sendRandomData() {
  // Generate random value between 1 and 10
  int randomValue = random(1, 11);  // random(min, max_exclusive)

  // Build JSON body
  String jsonBody = "{\"value\":" + String(randomValue) + ",\"deviceId\":\"" + String(DEVICE_ID) + "\"}";

  Serial.println("----------------------------------------");
  Serial.print("[SEND] Random value: ");
  Serial.println(randomValue);
  Serial.print("[SEND] JSON: ");
  Serial.println(jsonBody);

  // Send HTTPS POST request
  Serial.println("[HTTPS] Connecting...");
  httpClient.beginRequest();
  int err = httpClient.post(API_PATH);

  if (err != 0) {
    Serial.print("[HTTPS] Connection failed, error: ");
    Serial.println(err);
    httpClient.stop();
    return;
  }

  httpClient.sendHeader("Content-Type", "application/json");
  httpClient.sendHeader("Content-Length", jsonBody.length());
  httpClient.beginBody();
  httpClient.print(jsonBody);
  httpClient.endRequest();

  // Read response
  int statusCode = httpClient.responseStatusCode();
  String responseBody = httpClient.responseBody();

  Serial.print("[HTTPS] Status: ");
  Serial.println(statusCode);
  Serial.print("[HTTPS] Response: ");
  Serial.println(responseBody);

  if (statusCode == 201) {
    Serial.println("[OK] Data saved successfully!");
  } else {
    Serial.println("[WARN] Unexpected status code");
  }

  httpClient.stop();
  Serial.println("----------------------------------------\n");
}
