/*
 * SISTEMA DE CONTROL DE ACCESO VEHICULAR
 * Arduino - Control de Hardware
 * Archivo: control-acceso/hardware/arduino/main.ino
 */

#include <SoftwareSerial.h>
#include <Servo.h>
#include "config.h"

// Comunicación con ESP32-CAM
SoftwareSerial esp32Serial(2, 3); // RX, TX

// Objetos
Servo servoTranquera;

// Variables de control
bool procesoActivo = false;
unsigned long tiempoDeteccion = 0;
unsigned long ultimaDeteccion = 0;

// Estados del sistema
enum EstadoSistema { ESPERANDO, PROCESANDO, ABRIENDO, ERROR_ESTADO };
EstadoSistema estadoActual = ESPERANDO;

void setup() {
  Serial.begin(9600);
  esp32Serial.begin(115200);
  
  // Configurar pines
  pinMode(SENSOR_TRIG, OUTPUT);
  pinMode(SENSOR_ECHO, INPUT);
  pinMode(LED_STATUS, OUTPUT);
  pinMode(LED_ACCESO, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Configurar servo
  servoTranquera.attach(SERVO_PIN);
  cerrarTranquera();
  
  // Secuencia de inicio
  secuenciaInicializacion();
  
  Serial.println("========================================");
  Serial.println("SISTEMA DE CONTROL DE ACCESO V2.0");
  Serial.println("========================================");
  Serial.println("Arduino: Operativo");
  Serial.println("Distancia deteccion: " + String(DISTANCIA_DETECCION) + " cm");
  Serial.println("Esperando conexion ESP32-CAM...");
  
  // Verificar conexión ESP32-CAM
  verificarConexionESP32();
}

void loop() {
  // Actualizar indicadores
  actualizarIndicadores();
  
  // Medir distancia
  long distancia = medirDistancia();
  
  // Procesar según estado
  switch (estadoActual) {
    case ESPERANDO:
      procesarEspera(distancia);
      break;
    case PROCESANDO:
      procesarComandosESP32();
      verificarTimeout();
      break;
    case ABRIENDO:
      // Estado manejado por funciones específicas
      break;
    case ERROR_ESTADO:
      // Reiniciar tras error
      delay(5000);
      estadoActual = ESPERANDO;
      break;
  }
  
  // Debug cada 5 segundos
  static unsigned long ultimoDebug = 0;
  if (millis() - ultimoDebug > 5000) {
    Serial.println("Estado: " + obtenerNombreEstado() + " | Distancia: " + String(distancia) + "cm");
    ultimoDebug = millis();
  }
  
  delay(200);
}

long medirDistancia() {
  digitalWrite(SENSOR_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(SENSOR_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENSOR_TRIG, LOW);
  
  long duracion = pulseIn(SENSOR_ECHO, HIGH, 30000);
  if (duracion == 0) return 999; // Error en lectura
  
  return (duracion * 0.034) / 2;
}

void procesarEspera(long distancia) {
  if (distancia <= DISTANCIA_DETECCION && distancia >= 5) {
    if (millis() - ultimaDeteccion > COOLDOWN_DETECCION) {
      iniciarProcesoDeteccion(distancia);
    }
  }
}

void iniciarProcesoDeteccion(long distancia) {
  Serial.println("\n🚗 VEHICULO DETECTADO");
  Serial.println("Distancia: " + String(distancia) + " cm");
  
  estadoActual = PROCESANDO;
  procesoActivo = true;
  tiempoDeteccion = millis();
  ultimaDeteccion = millis();
  
  // Indicadores
  secuenciaDeteccion();
  
  // Comando a ESP32-CAM
  Serial.println("Enviando comando a ESP32-CAM...");
  esp32Serial.println("CAPTURAR_PLACA");
  esp32Serial.flush();
}

void procesarComandosESP32() {
  if (esp32Serial.available()) {
    String respuesta = esp32Serial.readStringUntil('\n');
    respuesta.trim();
    
    Serial.println("ESP32: " + respuesta);
    
    if (respuesta.startsWith("PLACA_AUTORIZADA:")) {
      String placa = respuesta.substring(17);
      ejecutarAccesoAutorizado(placa);
    }
    else if (respuesta.startsWith("PLACA_NO_AUTORIZADA:")) {
      String placa = respuesta.substring(20);
      ejecutarAccesoDenegado(placa);
    }
    else if (respuesta.startsWith("ERROR_")) {
      ejecutarErrorSistema(respuesta);
    }
    else if (respuesta == "PROCESANDO") {
      Serial.println("ESP32-CAM procesando...");
      parpadearLED(LED_STATUS, 1);
    }
    else if (respuesta == "CONECTADO") {
      Serial.println("✅ ESP32-CAM conectado");
    }
  }
}

void ejecutarAccesoAutorizado(String placa) {
  estadoActual = ABRIENDO;
  
  Serial.println("\n✅ ACCESO AUTORIZADO");
  Serial.println("Placa: " + placa);
  
  secuenciaExito();
  abrirTranquera();
  
  // Mantener abierta
  unsigned long inicio = millis();
  while (millis() - inicio < 7000) {
    digitalWrite(LED_ACCESO, HIGH);
    delay(100);
  }
  
  cerrarTranquera();
  finalizarProceso("AUTORIZADO", placa);
}

void ejecutarAccesoDenegado(String placa) {
  estadoActual = ABRIENDO;
  
  Serial.println("\n❌ ACCESO DENEGADO");
  Serial.println("Placa: " + placa);
  Serial.println("SMS enviado a seguridad");
  
  secuenciaAlerta();
  
  // Esperar autorización manual
  delay(4000);
  
  Serial.println("Apertura temporal autorizada");
  abrirTranquera();
  
  // Mantener abierta con alerta
  unsigned long inicio = millis();
  while (millis() - inicio < 6000) {
    digitalWrite(LED_ERROR, HIGH);
    delay(200);
    digitalWrite(LED_ERROR, LOW);
    delay(200);
  }
  
  cerrarTranquera();
  finalizarProceso("TEMPORAL", placa);
}

void ejecutarErrorSistema(String error) {
  Serial.println("\n❌ ERROR: " + error);
  
  secuenciaError();
  abrirTranquera();
  delay(5000);
  cerrarTranquera();
  
  finalizarProceso("ERROR", "N/A");
}

void verificarTimeout() {
  if (millis() - tiempoDeteccion > TIMEOUT_PROCESO) {
    Serial.println("TIMEOUT - Apertura de seguridad");
    ejecutarErrorSistema("TIMEOUT");
  }
}

void verificarConexionESP32() {
  Serial.println("Verificando ESP32-CAM...");
  
  for (int i = 0; i < 5; i++) {
    esp32Serial.println("PING");
    delay(1000);
    
    if (esp32Serial.available()) {
      String resp = esp32Serial.readStringUntil('\n');
      if (resp.indexOf("PONG") >= 0) {
        Serial.println("✅ ESP32-CAM conectado");
        return;
      }
    }
    Serial.println("Intento " + String(i + 1) + "/5...");
  }
  
  Serial.println("⚠️ ESP32-CAM no responde");
}

// === FUNCIONES DE CONTROL ===

void abrirTranquera() {
  Serial.println("🔓 ABRIENDO TRANQUERA");
  servoTranquera.write(90);
  delay(1000);
}

void cerrarTranquera() {
  Serial.println("🔒 CERRANDO TRANQUERA");
  servoTranquera.write(0);
  delay(1000);
}

// === SECUENCIAS DE INDICADORES ===

void secuenciaInicializacion() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_STATUS, HIGH);
    digitalWrite(LED_ACCESO, HIGH);
    digitalWrite(LED_ERROR, HIGH);
    tone(BUZZER_PIN, 1000, 200);
    delay(300);
    apagarTodosLEDs();
    delay(200);
  }
}

void secuenciaDeteccion() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_STATUS, HIGH);
    tone(BUZZER_PIN, 800, 150);
    delay(200);
    digitalWrite(LED_STATUS, LOW);
    delay(150);
  }
}

void secuenciaExito() {
  digitalWrite(LED_ACCESO, HIGH);
  tone(BUZZER_PIN, 1200, 500);
  delay(600);
  digitalWrite(LED_ACCESO, LOW);
}

void secuenciaAlerta() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_ERROR, HIGH);
    tone(BUZZER_PIN, 1800, 100);
    delay(150);
    digitalWrite(LED_ERROR, LOW);
    delay(100);
  }
}

void secuenciaError() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(LED_ERROR, HIGH);
    tone(BUZZER_PIN, 2200, 80);
    delay(100);
    digitalWrite(LED_ERROR, LOW);
    delay(80);
  }
}

void actualizarIndicadores() {
  static unsigned long ultimoParpadeo = 0;
  static bool estadoLED = false;
  
  if (millis() - ultimoParpadeo > 1000) {
    estadoLED = !estadoLED;
    ultimoParpadeo = millis();
    
    if (estadoActual == ESPERANDO) {
      digitalWrite(LED_STATUS, estadoLED);
    } else if (estadoActual == PROCESANDO) {
      digitalWrite(LED_STATUS, HIGH);
    }
  }
}

void parpadearLED(int pin, int veces) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(pin, HIGH);
    delay(150);
    digitalWrite(pin, LOW);
    delay(150);
  }
}

void apagarTodosLEDs() {
  digitalWrite(LED_STATUS, LOW);
  digitalWrite(LED_ACCESO, LOW);
  digitalWrite(LED_ERROR, LOW);
}

String obtenerNombreEstado() {
  switch (estadoActual) {
    case ESPERANDO: return "ESPERANDO";
    case PROCESANDO: return "PROCESANDO";
    case ABRIENDO: return "ABRIENDO";
    case ERROR_ESTADO: return "ERROR";
    default: return "DESCONOCIDO";
  }
}

void finalizarProceso(String resultado, String placa) {
  estadoActual = ESPERANDO;
  procesoActivo = false;
  apagarTodosLEDs();
  
  Serial.println("\n=== PROCESO COMPLETADO ===");
  Serial.println("Resultado: " + resultado);
  Serial.println("Placa: " + placa);
  Serial.println("Tiempo: " + String((millis() - tiempoDeteccion)/1000) + "s");
  Serial.println("===========================\n");
  
  delay(2000);
}