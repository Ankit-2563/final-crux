// ============================================================
// secrets.h — private config for ServerDataPush
// ============================================================
#ifndef SECRETS_H
#define SECRETS_H

// Server (public IP is fine)
const char SERVER_HOST[] = "15.206.125.247";
const int SERVER_PORT = 443;
const char REGISTER_API_PATH[] = "/api/hardware/register";
const char DATA_API_PATH[] = "/api/hardware/data";

// Device credentials (must match backend register/auth)
const char DEVICE_ID[] = "crux01";
const char DEVICE_SECRET[] = "jai-shree-krishna";
const char FIRMWARE_VERSION[] = "v1.0.0";

// Send interval
const unsigned long SEND_INTERVAL = 30000;

// Sensor pins from your wiring
// Temperature sensors (4 total) data pins: 4, 5, 16, 39
// You said: 1x DHT11 + 3x DHT22. Assign which pin is DHT11 below.
const int TEMP_PIN_1 = 4;   // DHT11 (default mapping; change if needed)
const int TEMP_PIN_2 = 5;   // DHT22
const int TEMP_PIN_3 = 16;  // DHT22
const int TEMP_PIN_4 = 39;  // DHT22
const int VOLTAGE_PIN = 35;
const int CURRENT_PIN = 36;

// ADC + calibration constants
const float ADC_REF_V = 3.3f;
const float ADC_MAX = 4095.0f;

// Voltage measurement (divider compensation)
const float VOLTAGE_DIVIDER_RATIO = 5.0f;

// Current measurement (ACS-like sensor defaults)
const float CURRENT_ZERO_V = 1.65f;
const float CURRENT_SENSITIVITY_V_PER_A = 0.100f;

// DHT read settings
const unsigned long DHT_READ_MIN_INTERVAL_MS = 2500; // DHT11/22 need ~2s between reads
const float TEMP_FALLBACK_C = 0.0f;                  // used only if all sensors fail

#endif
