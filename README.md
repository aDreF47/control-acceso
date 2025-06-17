# 🚗 Sistema de Control de Acceso Vehicular

Sistema automático de control de acceso para vehículos con reconocimiento de placas, base de datos en tiempo real y notificaciones SMS.

## ✨ Características

- 🚗 **Detección automática** de vehículos con sensor ultrasónico
- 📸 **Captura de imagen** con ESP32-CAM
- 🤖 **Reconocimiento OCR** de placas vehiculares
- 🔥 **Base de datos Firebase** en tiempo real
- 📱 **Alertas SMS** via Twilio para vehículos no autorizados
- 🚪 **Control automático** de tranquera con servo motor
- 📊 **Logs y estadísticas** completos

## 🔧 Hardware Necesario

| Componente | Cantidad | Precio Aprox. |
|------------|----------|---------------|
| Arduino Uno/Nano | 1 | $5 |
| ESP32-CAM | 1 | $8 |
| Sensor HC-SR04 | 1 | $3 |
| Servo SG90 | 1 | $3 |
| LEDs (Verde, Amarillo, Rojo) | 3 | $2 |
| Buzzer | 1 | $2 |
| Resistencias 220Ω | 3 | $1 |
| Protoboard y cables | 1 | $5 |
| **Total** | | **~$30** |

## 🌐 Servicios Cloud

- **Firebase Realtime Database** (Gratis)
- **Twilio SMS** (~$0.01 por SMS)
- **Hosting Backend** (Gratis en Render/Railway)

## 🚀 Instalación Rápida

### 1. Clonar repositorio
```bash
git clone https://github.com/tu-usuario/control-acceso.git
cd control-acceso
```

### 2. Configurar servicios
```bash
# Firebase: Crear proyecto en console.firebase.google.com
# Twilio: Crear cuenta en twilio.com
# Copiar credenciales a archivos de configuración
```

### 3. Programar microcontroladores
```bash
# Arduino IDE: Abrir hardware/arduino/main.ino
# Configurar WiFi y credenciales en config.h
# Subir código a Arduino y ESP32-CAM
```

### 4. Backend Python (Opcional)
```bash
cd backend
pip install -r requirements.txt
python app.py
```

### 5. Conectar hardware
Ver guía detallada en [hardware-setup.md](docs/hardware-setup.md)

## 📋 Configuración

### Arduino (hardware/arduino/config.h)
```cpp
#define WIFI_SSID "TU_WIFI"
#define WIFI_PASSWORD "TU_PASSWORD"
#define DISTANCIA_DETECCION 30  // cm
```

### ESP32-CAM (hardware/esp32-cam/config.h)
```cpp
#define FIREBASE_HOST "tu-proyecto.firebaseio.com"
#define FIREBASE_AUTH "tu-auth-token"
#define TWILIO_ACCOUNT_SID "ACxxxx"
#define TWILIO_AUTH_TOKEN "xxxx"
```

### Backend Python (backend/.env)
```bash
FIREBASE_CREDS_PATH=./firebase-credentials.json
TWILIO_ACCOUNT_SID=ACxxxx
TWILIO_AUTH_TOKEN=xxxx
PHONE_SEGURIDAD=+51987654321
```

## 🔄 Flujo del Sistema

1. **Arduino** detecta vehículo → Envía comando a **ESP32-CAM**
2. **ESP32-CAM** captura foto → Procesa OCR → Consulta **Firebase**
3. **Firebase** valida placa → Responde autorización
4. Si NO autorizado → **Twilio** envía SMS de alerta
5. **ESP32-CAM** → **Arduino** → Controla tranquera

## 📱 API Endpoints (Backend Python)

```bash
GET  /health              # Estado del sistema
POST /process_plate       # Procesar imagen de placa
POST /verify_plate        # Verificar placa específica
POST /add_resident        # Agregar nuevo residente
```

## 🐛 Troubleshooting

- **Arduino no detecta vehículos**: Verificar conexiones HC-SR04
- **ESP32-CAM no conecta WiFi**: Revisar credenciales en config.h  
- **OCR no funciona**: Verificar iluminación y ángulo de cámara
- **SMS no llegan**: Verificar credenciales Twilio

Ver más en [troubleshooting.md](docs/troubleshooting.md)

## 📊 Monitoreo

- **Firebase Console**: Ver datos en tiempo real
- **Monitor Serial**: Debug de Arduino/ESP32
- **Backend Logs**: Estado del sistema Python

## 🔒 Seguridad

- Base de datos protegida con reglas Firebase
- Comunicación HTTPS para APIs
- Validación de datos en todos los niveles

## 📈 Escalabilidad

- ✅ Múltiples cámaras (ESP32-CAM adicionales)
- ✅ Miles de residentes en Firebase
- ✅ App móvil para administración
- ✅ Reconocimiento facial adicional

## 🤝 Contribuir

1. Fork del repositorio
2. Crear branch: `git checkout -b feature/nueva-funcionalidad`
3. Commit: `git commit -m 'Agregar nueva funcionalidad'`
4. Push: `git push origin feature/nueva-funcionalidad`
5. Pull Request

## 📄 Licencia

MIT License - Ver [LICENSE](LICENSE) para más detalles.

## 📞 Soporte

- 📧 Email: soporte@control-acceso.com
- 💬 Discord: [Servidor del proyecto]
- 📱 WhatsApp: +51987654321

---

⭐ **Si te gusta el proyecto, dale una estrella en GitHub!** ⭐