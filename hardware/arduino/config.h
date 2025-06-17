/*
 * CONFIGURACIÓN ARDUINO
 * Archivo: control-acceso/hardware/arduino/config.h
 */

#ifndef CONFIG_H
#define CONFIG_H

// === PINES DE HARDWARE ===
#define SERVO_PIN 9           // Servo de la tranquera
#define SENSOR_TRIG 7         // Trigger del HC-SR04
#define SENSOR_ECHO 8         // Echo del HC-SR04
#define LED_STATUS 13         // LED amarillo - estado sistema
#define LED_ACCESO 12         // LED verde - acceso autorizado
#define LED_ERROR 10          // LED rojo - errores/alertas
#define BUZZER_PIN 11         // Buzzer para indicadores sonoros

// === PARÁMETROS DEL SISTEMA ===
#define DISTANCIA_DETECCION 30    // Distancia en cm para detectar vehículo
#define DISTANCIA_MINIMA 5        // Distancia mínima válida (filtrar ruido)
#define TIMEOUT_PROCESO 20000     // Timeout en ms para proceso completo
#define COOLDOWN_DETECCION 5000   // Tiempo en ms entre detecciones

// === CONFIGURACIÓN SERVO ===
#define SERVO_CERRADO 0           // Ángulo para tranquera cerrada
#define SERVO_ABIERTO 90          // Ángulo para tranquera abierta

// === TIEMPOS DE APERTURA ===
#define TIEMPO_ABIERTO_AUTORIZADO 7000  // 7 segundos para residentes
#define TIEMPO_ABIERTO_TEMPORAL 6000    // 6 segundos para temporales
#define TIEMPO_ABIERTO_EMERGENCIA 5000  // 5 segundos para emergencias

// === CONFIGURACIÓN COMUNICACIÓN ===
#define SERIAL_BAUD 9600          // Velocidad para debug
#define ESP32_SERIAL_BAUD 115200  // Velocidad comunicación ESP32

// === CONFIGURACIÓN DEBUG ===
#define DEBUG_ENABLED 1           // 1 = Activar debug, 0 = Desactivar
#define DEBUG_INTERVAL 5000       // Intervalo debug en ms

#endif