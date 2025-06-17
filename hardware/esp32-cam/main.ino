/*
 * SISTEMA DE CONTROL DE ACCESO VEHICULAR
 * ESP32-CAM - Procesamiento de Imagen
 * Archivo: control-acceso/hardware/esp32-cam/main.ino
 */

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Base64.h>
#include "esp_camera.h"
#include "config.h"

// Comunicación con Arduino
HardwareSerial arduinoSerial(2);

// Firebase
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// Variables del sistema
bool sistemaInicializado = false;
bool wifiConectado = false;
bool firebaseConectado = false;

// Configuración cámara AI-Thinker
camera_config_t cameraConfig = {
  .pin_pwdn = 32,
  .pin_reset = -1,
  .pin_xclk = 0,
  .pin_sscb_sda = 26,
  .pin_sscb_scl = 27,
  .pin_d7 = 35,
  .pin_d6 = 34,
  .pin_d5 = 39,
  .pin_d4 = 36,
  .pin_d3 = 21,
  .pin_d2 = 19,
  .pin_d1 = 18,
  .pin_d0 = 5,
  .pin_vsync = 25,
  .pin_href = 23,
  .pin_pclk = 22,
  .xclk_freq_hz = 20000000,
  .ledc_timer = LEDC_TIMER_0,
  .ledc_channel = LEDC_CHANNEL_0,
  .pixel_format = PIXFORMAT_JPEG,
  .frame_size = FRAMESIZE_SVGA,
  .jpeg_quality = 10,
  .fb_count = 2
};

void setup() {
  Serial.begin(115200);
  arduinoSerial.begin(115200, SERIAL_8N1, 3, 1);
  
  Serial.println("========================================");
  Serial.println("ESP32-CAM CONTROL DE ACCESO V2.0");
  Serial.println("========================================");
  
  inicializarSistema();
}

void inicializarSistema() {
  Serial.println("Inicializando componentes...");
  
  // 1. Inicializar cámara
  if (!inicializarCamara()) {
    Serial.println("ERROR: Fallo en cámara");
    return;
  }
  
  // 2. Conectar WiFi
  if (!conectarWiFi()) {
    Serial.println("ERROR: Fallo en WiFi");
    return;
  }
  
  // 3. Conectar Firebase
  if (!conectarFirebase()) {
    Serial.println("WARNING: Firebase no disponible");
  }
  
  sistemaInicializado = true;
  Serial.println("\n✅ ESP32-CAM inicializado");
  Serial.println("📸 Cámara: OK");
  Serial.println("🌐 WiFi: " + WiFi.localIP().toString());
  Serial.println("🔥 Firebase: " + String(firebaseConectado ? "OK" : "ERROR"));
  Serial.println("Esperando comandos Arduino...\n");
  
  // Notificar Arduino
  arduinoSerial.println("CONECTADO");
}

void loop() {
  // Procesar comandos Arduino
  if (arduinoSerial.available()) {
    String comando = arduinoSerial.readStringUntil('\n');
    comando.trim();
    
    Serial.println("Comando: " + comando);
    
    if (comando == "CAPTURAR_PLACA") {
      procesarCapturaDePlaca();
    } else if (comando == "PING") {
      arduinoSerial.println("PONG");
    }
  }
  
  // Verificar conexiones cada 30s
  static unsigned long ultimaVerificacion = 0;
  if (millis() - ultimaVerificacion > 30000) {
    verificarConexiones();
    ultimaVerificacion = millis();
  }
  
  delay(100);
}

bool inicializarCamara() {
  Serial.println("Inicializando cámara...");
  
  esp_err_t err = esp_camera_init(&cameraConfig);
  if (err != ESP_OK) {
    Serial.println("ERROR inicializando cámara");
    return false;
  }
  
  // Configurar parámetros
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 0);
  s->set_contrast(s, 0);
  s->set_saturation(s, 0);
  s->set_whitebal(s, 1);
  s->set_awb_gain(s, 1);
  s->set_exposure_ctrl(s, 1);
  s->set_gain_ctrl(s, 1);
  
  Serial.println("✅ Cámara configurada");
  return true;
}

bool conectarWiFi() {
  Serial.println("Conectando WiFi: " + String(WIFI_SSID));
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setSleep(false);
  
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConectado = true;
    Serial.println("\n✅ WiFi conectado");
    Serial.println("IP: " + WiFi.localIP().toString());
    return true;
  } else {
    Serial.println("\n❌ Error WiFi");
    return false;
  }
}

bool conectarFirebase() {
  Serial.println("Configurando Firebase...");
  
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  if (Firebase.ready()) {
    firebaseConectado = true;
    Serial.println("✅ Firebase conectado");
    registrarEventoSistema("esp32_iniciado");
    return true;
  } else {
    Serial.println("❌ Firebase error");
    return false;
  }
}

void procesarCapturaDePlaca() {
  if (!sistemaInicializado) {
    Serial.println("ERROR: Sistema no inicializado");
    arduinoSerial.println("ERROR_CAPTURA");
    return;
  }
  
  Serial.println("\n📸 PROCESANDO PLACA");
  arduinoSerial.println("PROCESANDO");
  
  // 1. Capturar imagen
  camera_fb_t * fb = capturarImagen();
  if (!fb) {
    arduinoSerial.println("ERROR_CAPTURA");
    return;
  }
  
  // 2. Procesar OCR
  String placaDetectada = procesarOCR(fb);
  esp_camera_fb_return(fb);
  
  if (placaDetectada == "") {
    arduinoSerial.println("ERROR_OCR");
    return;
  }
  
  // 3. Verificar en Firebase
  verificarPlacaEnFirebase(placaDetectada);
}

camera_fb_t* capturarImagen() {
  Serial.println("📷 Capturando imagen...");
  
  // Tomar varias fotos para mejor calidad
  for (int i = 0; i < 3; i++) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (i < 2) {
      esp_camera_fb_return(fb); // Descartar primeras
    } else {
      if (fb && fb->len > 0) {
        Serial.println("✅ Imagen capturada: " + String(fb->len) + " bytes");
        return fb;
      }
    }
    delay(100);
  }
  
  Serial.println("❌ Error captura");
  return nullptr;
}

String procesarOCR(camera_fb_t* fb) {
  Serial.println("🤖 Procesando OCR...");
  
  // OCR básico simulado para pruebas
  // En implementación real usar Tesseract o API externa
  
  String placasPrueba[] = {
    "ABC123", "DEF456", "GHI789", "JKL012", // Autorizadas
    "XYZ999", "UVW888", "RST777", "LMN666"  // No autorizadas
  };
  
  delay(1500); // Simular procesamiento
  
  // Retornar placa aleatoria para pruebas
  String placa = placasPrueba[random(0, 8)];
  Serial.println("✅ Placa extraída: " + placa);
  
  return placa;
}

void verificarPlacaEnFirebase(String placa) {
  Serial.println("🔍 Verificando: " + placa);
  
  if (!firebaseConectado) {
    Serial.println("⚠️ Firebase offline - Verificación local");
    verificarPlacaLocal(placa);
    return;
  }
  
  String path = "/residentes/" + placa;
  
  if (Firebase.RTDB.getString(&firebaseData, path)) {
    if (firebaseData.dataType() == "string" && firebaseData.stringData() != "null") {
      Serial.println("✅ Placa autorizada en Firebase");
      registrarAcceso(placa, "autorizado");
      arduinoSerial.println("PLACA_AUTORIZADA:" + placa);
    } else {
      placaNoAutorizada(placa);
    }
  } else {
    Serial.println("❌ Error Firebase: " + firebaseData.errorReason());
    verificarPlacaLocal(placa);
  }
}

void verificarPlacaLocal(String placa) {
  Serial.println("💾 Verificación local...");
  
  String placasAutorizadas[] = {"ABC123", "DEF456", "GHI789", "JKL012"};
  
  bool autorizada = false;
  for (int i = 0; i < 4; i++) {
    if (placasAutorizadas[i] == placa) {
      autorizada = true;
      break;
    }
  }
  
  if (autorizada) {
    Serial.println("✅ Placa autorizada (local)");
    arduinoSerial.println("PLACA_AUTORIZADA:" + placa);
  } else {
    placaNoAutorizada(placa);
  }
}

void placaNoAutorizada(String placa) {
  Serial.println("❌ Placa NO autorizada: " + placa);
  
  // Registrar temporal
  registrarVehiculoTemporal(placa);
  
  // Enviar SMS
  enviarAlertaSMS(placa);
  
  arduinoSerial.println("PLACA_NO_AUTORIZADA:" + placa);
}

void registrarAcceso(String placa, String tipo) {
  if (!firebaseConectado) return;
  
  Serial.println("💾 Registrando acceso...");
  
  String timestamp = String(WiFi.getTime());
  String path = "/accesos/" + timestamp;
  
  DynamicJsonDocument doc(512);
  doc["placa"] = placa;
  doc["tipo"] = tipo;
  doc["timestamp"] = timestamp;
  doc["fecha"] = obtenerFechaActual();
  doc["ubicacion"] = "entrada_principal";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  if (Firebase.RTDB.setString(&firebaseData, path, jsonString)) {
    Serial.println("✅ Acceso registrado");
  } else {
    Serial.println("❌ Error registro: " + firebaseData.errorReason());
  }
}

void registrarVehiculoTemporal(String placa) {
  Serial.println("💾 Registrando temporal...");
  
  if (firebaseConectado) {
    String timestamp = String(WiFi.getTime());
    String path = "/vehiculos_temporales/" + timestamp;
    
    DynamicJsonDocument doc(512);
    doc["placa"] = placa;
    doc["timestamp"] = timestamp;
    doc["fecha"] = obtenerFechaActual();
    doc["tipo"] = "no_autorizado";
    doc["alerta_enviada"] = true;
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    Firebase.RTDB.setString(&firebaseData, path, jsonString);
  }
}

void enviarAlertaSMS(String placa) {
  Serial.println("📱 Enviando SMS...");
  
  if (!wifiConectado) {
    Serial.println("❌ Sin WiFi - SMS no enviado");
    return;
  }
  
  HTTPClient http;
  String url = "https://api.twilio.com/2010-04-01/Accounts/" + String(TWILIO_ACCOUNT_SID) + "/Messages.json";
  http.begin(url);
  
  // Autenticación
  String auth = String(TWILIO_ACCOUNT_SID) + ":" + String(TWILIO_AUTH_TOKEN);
  String encodedAuth = base64::encode(auth);
  http.addHeader("Authorization", "Basic " + encodedAuth);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Mensaje
  String mensaje = "🚨 ALERTA SEGURIDAD 🚨\\n";
  mensaje += "Vehiculo NO autorizado\\n";
  mensaje += "Placa: " + placa + "\\n";
  mensaje += "Hora: " + obtenerFechaActual() + "\\n";
  mensaje += "Ubicacion: Entrada Principal";
  
  String payload = "To=" + String(PHONE_SEGURIDAD) + 
                   "&From=" + String(TWILIO_PHONE_FROM) + 
                   "&Body=" + mensaje;
  
  int httpCode = http.POST(payload);
  
  if (httpCode == 201) {
    Serial.println("✅ SMS enviado");
  } else {
    Serial.println("❌ Error SMS: " + String(httpCode));
  }
  
  http.end();
}

void registrarEventoSistema(String evento) {
  if (!firebaseConectado) return;
  
  String timestamp = String(WiFi.getTime());
  String path = "/eventos_sistema/" + timestamp;
  
  DynamicJsonDocument doc(256);
  doc["evento"] = evento;
  doc["timestamp"] = timestamp;
  doc["dispositivo"] = "ESP32-CAM";
  doc["ip"] = WiFi.localIP().toString();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Firebase.RTDB.setString(&firebaseData, path, jsonString);
}

void verificarConexiones() {
  // Verificar WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi desconectado");
    wifiConectado = false;
    conectarWiFi();
  }
  
  // Verificar Firebase
  if (wifiConectado && !Firebase.ready()) {
    Serial.println("⚠️ Firebase desconectado");
    firebaseConectado = false;
    conectarFirebase();
  }
}

String obtenerFechaActual() {
  time_t now = WiFi.getTime();
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  
  char buffer[64];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}