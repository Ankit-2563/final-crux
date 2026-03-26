// ============================================================
// config.h — Server Configuration for HTTPSTest
// ============================================================
// IMPORTANT: Update SERVER_HOST with your EC2 public IP after deployment

#ifndef CONFIG_H
#define CONFIG_H

// Server connection
const char SERVER_HOST[] = "YOUR_EC2_PUBLIC_IP";   // <-- REPLACE with your EC2 IP, e.g. "54.123.45.67"
const int  SERVER_PORT   = 443;                     // HTTPS port

// Device identity
const char DEVICE_ID[]   = "VVM601";

// API endpoint path
const char API_PATH[]    = "/api/test";

// How often to send data (in milliseconds)
const unsigned long SEND_INTERVAL = 10000;  // 10 seconds

#endif
