/*
 * CONFIGURACIÓN ESP32-CAM
 * Archivo: control-acceso/hardware/esp32-cam/config.h
 */

#ifndef CONFIG_H
#define CONFIG_H

// === CONFIGURACIÓN WIFI ===
#define WIFI_SSID "TU_WIFI_SSID"
#define WIFI_PASSWORD "TU_WIFI_PASSWORD"

// === CONFIGURACIÓN FIREBASE ===
#define FIREBASE_HOST "tu-proyecto-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "tu-database-secret-key"

// === CONFIGURACIÓN TWILIO ===
#define TWILIO_ACCOUNT_SID "ACxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define TWILIO_AUTH_TOKEN "tu_auth_token_aqui"
#define TWILIO_PHONE_FROM "+1234567890"  // Tu número Twilio
#define PHONE_SEGURIDAD "+51987654321"   // Número de seguridad

// === CONFIGURACIÓN OCR ===
#define OCR_API_URL "https://api.ocr.space/parse/image"
#define OCR_API_KEY "tu_ocr_api_key"
#define OCR_ENABLED 1  // 1 = OCR real, 0 = simulado

// === CONFIGURACIÓN CÁMARA ===
#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27
#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

// === CONFIGURACIÓN COMUNICACIÓN ===
#define SERIAL_BAUD 115200
#define ARDUINO_RX_PIN 3
#define ARDUINO_TX_PIN 1

// === CONFIGURACIÓN SISTEMA ===
#define TIMEOUT_WIFI 20000      // Timeout WiFi en ms
#define TIMEOUT_FIREBASE 10000  // Timeout Firebase en ms
#define TIMEOUT_SMS 15000       // Timeout SMS en ms
#define REINTENTOS_MAXIMOS 3    // Reintentos para operaciones

// === BASE DE DATOS LOCAL (BACKUP) ===
// Placas autorizadas en caso de que Firebase no esté disponible
#define PLACAS_LOCALES_COUNT 4
const String PLACAS_AUTORIZADAS_LOCAL[PLACAS_LOCALES_COUNT] = {
  "ABC123", "DEF456", "GHI789", "JKL012"
};

// === CONFIGURACIÓN DEBUG ===
#define DEBUG_ENABLED 1
#define DEBUG_VERBOSE 0  // 1 = Debug detallado

#endif