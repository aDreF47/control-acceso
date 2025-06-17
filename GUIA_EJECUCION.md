# 🚀 Guía de Ejecución del Sistema

## ⚡ Paso 1: Verificación Hardware

### Checklist Pre-Encendido
```
□ Arduino conectado via USB o fuente 5V
□ ESP32-CAM con alimentación 5V estable
□ HC-SR04: VCC→5V, TRIG→Pin7, ECHO→Pin8, GND→GND
□ Servo: Rojo→5V, Marrón→GND, Naranja→Pin9
□ LEDs con resistencias: Pin10(Rojo), Pin12(Verde), Pin13(Amarillo)
□ Buzzer: +→Pin11, -→GND
□ Comunicación: Arduino Pin2↔ESP32 Pin1, Arduino Pin3↔ESP32 Pin3
□ Sin cortocircuitos visibles
```

## 📱 Paso 2: Configurar Servicios Cloud

### Firebase
```bash
1. Ir a: https://console.firebase.google.com
2. Crear proyecto: "control-acceso"
3. Realtime Database → Crear base de datos
4. Reglas: Modo prueba (permitir lectura/escritura)
5. Importar: firebase/database-structure.json
6. Configuración → Cuentas de servicio → Generar clave
7. Descargar: firebase-credentials.json
```

### Twilio
```bash
1. Crear cuenta: https://twilio.com
2. Console → Account Info:
   - Account SID: ACxxxxxxxx
   - Auth Token: xxxxxxxx
3. Phone Numbers → Comprar número (+$1/mes)
4. Verificar número de destino en: https://console.twilio.com/us1/develop/phone-numbers/manage/verified
```

## 🔧 Paso 3: Configurar Códigos

### Arduino (hardware/arduino/config.h)
```cpp
// Ajustar solo si necesario
#define DISTANCIA_DETECCION 30  // cm
#define TIMEOUT_PROCESO 20000   // ms
```

### ESP32-CAM (hardware/esp32-cam/config.h)
```cpp
// COMPLETAR CON TUS DATOS:
#define WIFI_SSID "TU_WIFI_AQUI"
#define WIFI_PASSWORD "TU_PASSWORD_AQUI"
#define FIREBASE_HOST "tu-proyecto-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "copiarlo-de-firebase-config"
#define TWILIO_ACCOUNT_SID "ACxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define TWILIO_AUTH_TOKEN "tu_auth_token_de_twilio"
#define TWILIO_PHONE_FROM "+1234567890"  // Tu número Twilio
#define PHONE_SEGURIDAD "+51987654321"   // Número destino
```

## 💻 Paso 4: Programar Microcontroladores

### Arduino IDE Setup
```bash
1. Instalar librerías:
   - Servo (ya incluida)
   - FirebaseESP32 by Mobizt
   - ArduinoJson by Benoit Blanchon

2. Configurar ESP32:
   - Archivo → Preferencias
   - URLs: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   - Herramientas → Placa → Gestor → Instalar "esp32"
```

### Programar Arduino
```bash
1. Conectar Arduino via USB
2. Herramientas → Placa → Arduino Uno
3. Herramientas → Puerto → COMx (Windows) / /dev/ttyUSBx (Linux)
4. Abrir: hardware/arduino/main.ino
5. Verificar (✓) → Subir (→)
6. Monitor Serial (9600 baud): Verificar "Sistema iniciado"
```

### Programar ESP32-CAM
```bash
1. IMPORTANTE: Modo programación ESP32-CAM
   - Conectar GPIO 0 → GND
   - Conectar FTDI: VCC→3.3V, GND→GND, TX→RX, RX→TX
   - Alimentar ESP32-CAM con 5V externo
   - Presionar RESET

2. Arduino IDE:
   - Herramientas → Placa → AI Thinker ESP32-CAM
   - Herramientas → Puerto → COMx del FTDI
   - Abrir: hardware/esp32-cam/main.ino
   - Verificar config.h completado
   - Subir código

3. Modo ejecución:
   - Desconectar GPIO 0 de GND
   - Presionar RESET
   - Monitor Serial (115200 baud): Ver "WiFi conectado"
```

## 🚀 Paso 5: Ejecutar Sistema

### Secuencia de Encendido
```bash
1. Encender Arduino (LED amarillo parpadeando)
2. Encender ESP32-CAM (verificar WiFi en monitor serial)
3. Verificar comunicación:
   - Arduino: "ESP32-CAM conectado"
   - ESP32-CAM: "CONECTADO"
```

### Logs de Verificación

#### Monitor Serial Arduino (9600 baud)
```
========================================
SISTEMA DE CONTROL DE ACCESO V2.0
========================================
Arduino: Operativo
Distancia deteccion: 30 cm
Esperando conexion ESP32-CAM...
Verificando ESP32-CAM...
✅ ESP32-CAM conectado
Estado: ESPERANDO | Distancia: 157cm
```

#### Monitor Serial ESP32-CAM (115200 baud)
```
========================================
ESP32-CAM CONTROL DE ACCESO V2.0
========================================
Inicializando componentes...
✅ Cámara configurada
Conectando WiFi: TU_WIFI
✅ WiFi conectado
IP: 192.168.1.150
✅ Firebase conectado
Esperando comandos Arduino...
```

## 🧪 Paso 6: Probar Funcionamiento

### Prueba 1: Detección de Vehículo
```bash
1. Acercar objeto al sensor HC-SR04 (<30cm)
2. Verificar secuencia:
   ✅ LEDs parpadean
   ✅ Buzzer suena
   ✅ Monitor Arduino: "VEHICULO DETECTADO"
   ✅ Monitor ESP32: "PROCESANDO PLACA"
```

### Prueba 2: Proceso Completo
```bash
Monitor Arduino:
🚗 VEHICULO DETECTADO
Distancia: 15 cm
Enviando comando a ESP32-CAM...
ESP32: PROCESANDO
ESP32: PLACA_AUTORIZADA:ABC123
✅ ACCESO AUTORIZADO
Placa: ABC123
🔓 ABRIENDO TRANQUERA
🔒 CERRANDO TRANQUERA
Proceso completado
```

### Prueba 3: Vehículo No Autorizado
```bash
Monitor ESP32-CAM:
📸 PROCESANDO PLACA
✅ Placa extraída: XYZ999
❌ Placa NO autorizada: XYZ999
💾 Registrando temporal...
📱 Enviando SMS...
✅ SMS enviado

Monitor Arduino:
❌ ACCESO DENEGADO
Placa: XYZ999
SMS enviado a seguridad
Apertura temporal autorizada
```

## 📱 Paso 7: Verificar Servicios

### Firebase Console
```bash
1. Ir a: https://console.firebase.google.com
2. Tu proyecto → Realtime Database
3. Verificar datos:
   - /accesos/timestamp: registros de entradas
   - /vehiculos_temporales/: no autorizados
   - /eventos_sistema/: logs del sistema
```

### SMS Twilio
```bash
1. Verificar recepción en número configurado
2. Mensaje típico:
   "🚨 ALERTA SEGURIDAD 🚨
   Vehiculo NO autorizado
   Placa: XYZ999
   Hora: 2024-06-16 14:30:15
   Ubicacion: Entrada Principal"
```

## 🐍 Paso 8: Backend Python (Opcional)

### Instalación
```bash
cd backend
python -m venv venv
source venv/bin/activate  # Linux/Mac
# venv\Scripts\activate   # Windows

pip install -r requirements.txt

# Instalar Tesseract:
# Ubuntu: sudo apt-get install tesseract-ocr
# Windows: Descargar desde GitHub UB-Mannheim/tesseract
# macOS: brew install tesseract
```

### Configuración
```bash
cp .env.example .env
# Editar .env con credenciales:
FIREBASE_CREDS_PATH=./firebase-credentials.json
FIREBASE_DB_URL=https://tu-proyecto-default-rtdb.firebaseio.com/
TWILIO_ACCOUNT_SID=ACxxxx
TWILIO_AUTH_TOKEN=xxxx
PHONE_SEGURIDAD=+51987654321
```

### Ejecutar
```bash
python app.py

# Servidor en: http://localhost:5000
# Probar: http://localhost:5000/health
```

## 🔧 Paso 9: Ajustes Finos

### Calibrar Sensor
```cpp
// En hardware/arduino/config.h
#define DISTANCIA_DETECCION 25  // Reducir si muy sensible
#define DISTANCIA_DETECCION 35  // Aumentar si no detecta
```

### Calibrar Cámara
```bash
1. Ajustar ángulo para enfocar zona de placas
2. Verificar iluminación (evitar contraluz)
3. Altura recomendada: 1.5-2m del suelo
```

### Calibrar Servo
```cpp
// Si tranquera no abre/cierra completamente
#define SERVO_CERRADO 0    // Ajustar entre -10 a 10
#define SERVO_ABIERTO 90   // Ajustar entre 80 a 100
```

## 🚨 Paso 10: Resolución de Problemas

### Problema: Arduino no detecta vehículos
```bash
Solución:
1. Verificar conexiones HC-SR04
2. Monitor Serial: verificar lecturas de distancia
3. Ajustar DISTANCIA_DETECCION en config.h
4. Verificar alimentación 5V estable
```

### Problema: ESP32-CAM no conecta WiFi
```bash
Solución:
1. Verificar SSID y password en config.h
2. Verificar red 2.4GHz (no 5GHz)
3. Verificar alimentación 5V estable (NO 3.3V)
4. Verificar GPIO 0 desconectado en ejecución
```

### Problema: No llegan SMS
```bash
Solución:
1. Verificar credenciales Twilio en config.h
2. Verificar número verificado en Twilio Console
3. Verificar créditos en cuenta Twilio
4. Verificar formato número: +51987654321
```

### Problema: Firebase no funciona
```bash
Solución:
1. Verificar FIREBASE_HOST en config.h
2. Verificar FIREBASE_AUTH correcto
3. Verificar reglas de base de datos
4. Verificar conexión WiFi estable
```

## 📊 Monitoreo Continuo

### Indicadores Normales
```bash
✅ LED Amarillo: Parpadeando (esperando)
✅ Monitor Arduino: "Estado: ESPERANDO | Distancia: XXXcm"
✅ Monitor ESP32: "💓 ESP32-CAM operativo"
✅ Firebase: Nuevos registros apareciendo
```

### Indicadores de Problema
```bash
❌ LED Rojo: Encendido fijo (error)
❌ Monitor: "ERROR" en mensajes
❌ Sin respuesta de ESP32-CAM
❌ Sin registros en Firebase
```

## 🎯 Uso Normal del Sistema

### Flujo Típico
```bash
1. Vehículo se acerca (<30cm del sensor)
2. Sistema detecta → LEDs parpadean + buzzer
3. ESP32-CAM captura imagen → procesa placa
4. Consulta Firebase → determina autorización
5. Si autorizado: Abre tranquera 7 segundos
6. Si no autorizado: Envía SMS + abre 6 segundos
7. Sistema regresa a espera
```

### Agregar Nuevos Residentes
```bash
Opción 1 - Firebase Console:
1. Ir a /residentes/
2. Agregar clave: "ABC123"
3. Valor: {"nombre": "Juan Pérez", "departamento": "101", "activo": true}

Opción 2 - API Backend:
curl -X POST http://localhost:5000/add_resident \
  -H "Content-Type: application/json" \
  -d '{"plate": "ABC123", "name": "Juan Pérez", "apartment": "101"}'
```

---

## 🎉 ¡Sistema Funcionando!

**El sistema está listo cuando veas:**
- ✅ Arduino detectando distancias
- ✅ ESP32-CAM conectado a WiFi y Firebase  
- ✅ Comunicación entre microcontroladores
- ✅ Tranquera responde a detección
- ✅ SMS llegando para vehículos no autorizados
- ✅ Registros apareciendo en Firebase

**¡Tu sistema de control de acceso está operativo! 🚀**