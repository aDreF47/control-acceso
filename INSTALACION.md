# 🚀 Guía de Instalación - Control de Acceso

## 📋 Requisitos Previos

### Hardware
- ✅ Arduino Uno/Nano
- ✅ ESP32-CAM
- ✅ Sensor HC-SR04
- ✅ Servo SG90
- ✅ LEDs (Verde, Amarillo, Rojo)
- ✅ Buzzer
- ✅ Resistencias 220Ω
- ✅ Cables y protoboard

### Software
- ✅ Arduino IDE
- ✅ Python 3.8+ (para backend opcional)
- ✅ Cuenta Firebase
- ✅ Cuenta Twilio

## 🔧 Paso 1: Configurar Servicios

### Firebase
1. Ve a [Firebase Console](https://console.firebase.google.com)
2. Crear nuevo proyecto: `control-acceso`
3. Habilitar **Realtime Database**
4. Configurar reglas:
```json
{
  "rules": {
    ".read": "auth != null",
    ".write": "auth != null"
  }
}
```
5. Ir a **Configuración del proyecto** → **Cuentas de servicio**
6. Generar nueva clave privada
7. Descargar `firebase-credentials.json`

### Twilio
1. Crear cuenta en [Twilio.com](https://twilio.com)
2. Ir al **Console Dashboard**
3. Copiar **Account SID** y **Auth Token**
4. Comprar número de teléfono (+$1/mes)
5. Verificar número de destino

## 🔌 Paso 2: Conexiones Hardware

### Arduino Uno
```
Pin 2  → ESP32-CAM TX (comunicación)
Pin 3  → ESP32-CAM RX (comunicación)
Pin 7  → HC-SR04 TRIG
Pin 8  → HC-SR04 ECHO
Pin 9  → Servo SG90 Signal (naranja)
Pin 10 → LED Rojo + Resistencia → GND
Pin 11 → Buzzer + → GND (-)
Pin 12 → LED Verde + Resistencia → GND
Pin 13 → LED Amarillo + Resistencia → GND

5V     → HC-SR04 VCC, Servo VCC (rojo)
GND    → Todos los GND, Servo GND (marrón)
```

### ESP32-CAM
```
Pin 1  → Arduino Pin 3 (TX)
Pin 3  → Arduino Pin 2 (RX)
5V     → Fuente externa 5V
GND    → Arduino GND
```

## 💻 Paso 3: Programar Microcontroladores

### Instalar Librerías Arduino IDE
```
1. Abrir Arduino IDE
2. Ir a Herramientas → Administrador de Librerías
3. Instalar:
   - FirebaseESP32 by Mobizt
   - ArduinoJson by Benoit Blanchon
   - Servo (incluida)
```

### Configurar ESP32 en Arduino IDE
```
1. Archivo → Preferencias
2. URLs adicionales de gestor de tarjetas:
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
3. Herramientas → Placa → Gestor de tarjetas
4. Instalar "esp32 by Espressif Systems"
5. Seleccionar: AI Thinker ESP32-CAM
```

### Programar Arduino
```bash
1. Abrir: hardware/arduino/main.ino
2. Editar: hardware/arduino/config.h
   - Configurar pines si es necesario
3. Seleccionar placa: Arduino Uno
4. Subir código
```

### Programar ESP32-CAM
```bash
1. Abrir: hardware/esp32-cam/main.ino
2. Editar: hardware/esp32-cam/config.h
   - WIFI_SSID: "TU_WIFI"
   - WIFI_PASSWORD: "TU_PASSWORD"
   - FIREBASE_HOST: "tu-proyecto-default-rtdb.firebaseio.com"
   - FIREBASE_AUTH: "tu-database-secret"
   - TWILIO_ACCOUNT_SID: "ACxxxx"
   - TWILIO_AUTH_TOKEN: "xxxx"
   - PHONE_SEGURIDAD: "+51987654321"

3. Conectar ESP32-CAM en modo programación:
   - Conectar GPIO 0 a GND
   - Conectar FTDI (3.3V)
   - Presionar RESET
   
4. Seleccionar:
   - Placa: AI Thinker ESP32-CAM
   - Puerto: COM correspondiente
   
5. Subir código
6. Desconectar GPIO 0 de GND
7. Presionar RESET para ejecutar
```

## 🐍 Paso 4: Backend Python (Opcional)

### Instalar Python y dependencias
```bash
# Crear entorno virtual
python -m venv venv

# Activar entorno
# Windows:
venv\Scripts\activate
# Linux/Mac:
source venv/bin/activate

# Instalar dependencias
cd backend
pip install -r requirements.txt
```

### Instalar Tesseract OCR
```bash
# Ubuntu/Debian:
sudo apt-get install tesseract-ocr tesseract-ocr-spa

# Windows:
# Descargar de: https://github.com/UB-Mannheim/tesseract/wiki
# Agregar al PATH del sistema

# macOS:
brew install tesseract tesseract-lang
```

### Configurar variables de entorno
```bash
# Copiar plantilla
cp .env.example .env

# Editar .env con tus credenciales:
FIREBASE_CREDS_PATH=./firebase-credentials.json
FIREBASE_DB_URL=https://tu-proyecto-default-rtdb.firebaseio.com/
TWILIO_ACCOUNT_SID=ACxxxx
TWILIO_AUTH_TOKEN=xxxx
TWILIO_PHONE_FROM=+1234567890
PHONE_SEGURIDAD=+51987654321
```

### Ejecutar backend
```bash
python app.py

# El servidor estará en: http://localhost:5000
```

## 🔥 Paso 5: Configurar Firebase

### Crear estructura de datos
```bash
1. Ir a Firebase Console → Realtime Database
2. Importar estructura desde: firebase/database-structure.json
```

### Agregar residentes de prueba
```json
{
  "residentes": {
    "ABC123": {
      "nombre": "Juan Pérez",
      "departamento": "101",
      "telefono": "+51987654321",
      "activo": true
    },
    "DEF456": {
      "nombre": "María González", 
      "departamento": "205",
      "telefono": "+51987654322",
      "activo": true
    }
  }
}
```

## ✅ Paso 6: Pruebas del Sistema

### Verificar comunicación
```bash
1. Abrir Monitor Serial Arduino (9600 baud)
2. Abrir Monitor Serial ESP32-CAM (115200 baud)
3. Verificar mensajes:
   - Arduino: "Sistema iniciado"
   - ESP32-CAM: "WiFi conectado" + "Firebase conectado"
   - Arduino: "ESP32-CAM conectado"
```

### Probar detección
```bash
1. Acercar objeto al sensor HC-SR04 (< 30cm)
2. Verificar secuencia:
   - LEDs parpadean
   - Buzzer suena
   - ESP32-CAM procesa
   - Tranquera se mueve
   - SMS enviado (si no autorizado)
```

### Verificar Firebase
```bash
1. Ir a Firebase Console → Realtime Database
2. Ver datos en tiempo real:
   - /accesos (registros de entrada)
   - /vehiculos_temporales (no autorizados)
   - /eventos_sistema (logs del sistema)
```

## 🐛 Solución de Problemas

### Arduino no detecta vehículos
- ✅ Verificar conexiones HC-SR04
- ✅ Verificar alimentación 5V
- ✅ Revisar DISTANCIA_DETECCION en config.h

### ESP32-CAM no conecta WiFi
- ✅ Verificar SSID y password en config.h
- ✅ Verificar red 2.4GHz (no 5GHz)
- ✅ Verificar alimentación 5V estable

### Firebase no funciona
- ✅ Verificar FIREBASE_HOST en config.h
- ✅ Verificar FIREBASE_AUTH válido
- ✅ Verificar reglas de base de datos

### SMS no llegan
- ✅ Verificar credenciales Twilio
- ✅ Verificar número verificado en Twilio
- ✅ Verificar créditos en cuenta Twilio

### Backend Python errores
- ✅ Verificar firebase-credentials.json presente
- ✅ Verificar Tesseract instalado
- ✅ Verificar variables en .env

## 📱 Uso del Sistema

### Agregar nuevo residente
```bash
# Via API Backend:
curl -X POST http://localhost:5000/add_resident \
  -H "Content-Type: application/json" \
  -d '{
    "plate": "GHI789",
    "name": "Carlos Rodríguez", 
    "apartment": "312",
    "phone": "+51987654323"
  }'

# Via Firebase Console:
# Agregar manualmente en /residentes/
```

### Monitorear sistema
- **Firebase Console**: Datos en tiempo real
- **Monitor Serial**: Debug Arduino/ESP32
- **Backend Logs**: Estado Python backend

## 🚀 Despliegue Producción

### Backend en la nube
```bash
# Render.com (gratis)
1. Conectar repositorio GitHub
2. Configurar variables de entorno
3. Deploy automático

# Railway.app (gratis)
1. Importar desde GitHub
2. Configurar .env
3. Deploy
```

### Mantenimiento
- Verificar créditos Twilio mensualmente
- Backup base de datos Firebase
- Actualizar firmware periódicamente
- Limpiar logs antiguos

## 💰 Costos Estimados

### Hardware: ~$30 (una vez)
### Servicios mensuales:
- Firebase: $0 (plan gratuito)
- Twilio: ~$0.01 por SMS
- Hosting: $0 (planes gratuitos)

**Total mensual: < $5**

---

🎉 **¡Sistema listo para funcionar!** 🎉