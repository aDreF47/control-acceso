# 🔌 Guía de Conexiones Hardware

## 📐 Diagrama de Conexiones

```
                    SISTEMA DE CONTROL DE ACCESO
                         
    ┌─────────────────┐                    ┌─────────────────┐
    │   ARDUINO UNO   │◄──Serial─────────►│   ESP32-CAM     │
    │                 │                    │                 │
    │  Pin 2 ── RX ───┼────────────────────┼─── TX (Pin 1)   │
    │  Pin 3 ── TX ───┼────────────────────┼─── RX (Pin 3)   │
    │                 │                    │                 │
    │  Pin 7 ─────────┼───► HC-SR04 TRIG   │                 │
    │  Pin 8 ─────────┼───► HC-SR04 ECHO   │                 │
    │  Pin 9 ─────────┼───► Servo Signal   │                 │
    │  Pin 10 ────────┼───► LED Rojo       │   📸 Cámara     │
    │  Pin 11 ────────┼───► Buzzer         │   📶 WiFi       │
    │  Pin 12 ────────┼───► LED Verde      │   🔥 Firebase   │
    │  Pin 13 ────────┼───► LED Amarillo   │   📱 Twilio     │
    │                 │                    │                 │
    │  5V ────────────┼─────────┬──────────┼─── 5V           │
    │  GND ───────────┼─────────┼──────────┼─── GND          │
    └─────────────────┘         │          └─────────────────┘
                                │
         ┌──────────────────────┴──────────────────────┐
         │                                             │
    ┌────▼────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐ │
    │HC-SR04  │  │Servo SG90│  │  LEDs   │  │ Buzzer  │ │
    │ VCC ────┤  │VCC ─────┤  │ +220Ω   │  │   +     │ │
    │ TRIG ───┤  │Signal ──┤  │         │  │         │ │
    │ ECHO ───┤  │GND ─────┤  │ GND ────│  │  GND ───│ │
    │ GND ────┤  └─────────┘  └─────────┘  └─────────┘ │
    └─────────┘                                        │
                                                       │
                    🔌 FUENTE 5V 2A ─────────────────┘
```

## 🔧 Conexiones Detalladas

### Arduino Uno
| Pin Arduino | Conexión | Componente | Color Cable |
|-------------|----------|------------|-------------|
| Pin 2 | RX | ESP32-CAM TX | Amarillo |
| Pin 3 | TX | ESP32-CAM RX | Naranja |
| Pin 7 | Digital Out | HC-SR04 TRIG | Verde |
| Pin 8 | Digital In | HC-SR04 ECHO | Azul |
| Pin 9 | PWM | Servo Signal | Naranja |
| Pin 10 | Digital Out | LED Rojo + R220Ω | Rojo |
| Pin 11 | Digital Out | Buzzer + | Morado |
| Pin 12 | Digital Out | LED Verde + R220Ω | Verde |
| Pin 13 | Digital Out | LED Amarillo + R220Ω | Amarillo |
| 5V | Alimentación | VCC Componentes | Rojo |
| GND | Tierra | GND Componentes | Negro |

### ESP32-CAM (AI-Thinker)
| Pin ESP32-CAM | Conexión | Destino | Notas |
|---------------|----------|---------|-------|
| Pin 1 (TX) | Serial | Arduino Pin 2 | Comunicación |
| Pin 3 (RX) | Serial | Arduino Pin 3 | Comunicación |
| 5V | Alimentación | Fuente 5V | **Importante: 5V estable** |
| GND | Tierra | Arduino GND | Común |

### HC-SR04 (Sensor Ultrasónico)
| Pin HC-SR04 | Conexión | Arduino Pin | Función |
|-------------|----------|-------------|---------|
| VCC | 5V | 5V | Alimentación |
| TRIG | Digital | Pin 7 | Trigger (salida) |
| ECHO | Digital | Pin 8 | Echo (entrada) |
| GND | Tierra | GND | Común |

### Servo SG90
| Cable Servo | Conexión | Arduino Pin | Función |
|-------------|----------|-------------|---------|
| Rojo | VCC | 5V | Alimentación |
| Marrón/Negro | GND | GND | Tierra |
| Naranja/Amarillo | Signal | Pin 9 | Control PWM |

### LEDs con Resistencias
| LED | Arduino Pin | Resistencia | Función |
|-----|-------------|-------------|---------|
| Rojo | Pin 10 | 220Ω | Error/Alerta |
| Verde | Pin 12 | 220Ω | Acceso Autorizado |
| Amarillo | Pin 13 | 220Ω | Estado Sistema |

```
Arduino Pin ──► [220Ω] ──► LED(+) ──► GND
```

### Buzzer
| Terminal | Conexión | Arduino Pin |
|----------|----------|-------------|
| + (Positivo) | Digital | Pin 11 |
| - (Negativo) | GND | GND |

## ⚡ Alimentación del Sistema

### Opción 1: USB Arduino (Desarrollo)
```
USB → Arduino → 5V/GND → Componentes
```
**Limitación**: Corriente limitada (~500mA)

### Opción 2: Fuente Externa (Recomendado)
```
Fuente 5V 2A → Jack Arduino + ESP32-CAM
```
**Ventajas**: Corriente suficiente, más estable

### Consideraciones de Alimentación
- ⚠️ **ESP32-CAM requiere 5V estable** (no 3.3V)
- ⚠️ **Servo puede consumir hasta 1A** en movimiento
- ⚠️ **Total estimado**: ~1.5A máximo
- ✅ **Fuente recomendada**: 5V 2A mínimo

## 🔌 Programación ESP32-CAM

### Modo Programación
```
1. Conectar GPIO 0 → GND (activar bootloader)
2. Conectar FTDI USB-Serial (3.3V):
   - FTDI VCC → ESP32 3.3V (NO usar para alimentar)
   - FTDI GND → ESP32 GND  
   - FTDI TX → ESP32 RX
   - FTDI RX → ESP32 TX
3. Conectar alimentación 5V externa
4. Presionar RESET en ESP32-CAM
5. Subir código desde Arduino IDE
6. Desconectar GPIO 0 de GND
7. Presionar RESET para ejecutar
```

### Modo Ejecución
```
1. Alimentación 5V conectada
2. GPIO 0 desconectado (libre)
3. FTDI desconectado (opcional)
4. Comunicación con Arduino via pines 1,3
```

## 🛠️ Lista de Materiales

### Componentes Principales
| Componente | Cantidad | Precio Aprox. | Link Ejemplo |
|------------|----------|---------------|--------------|
| Arduino Uno R3 | 1 | $5 | AliExpress/Amazon |
| ESP32-CAM AI-Thinker | 1 | $8 | AliExpress |
| Sensor HC-SR04 | 1 | $3 | Universal |
| Servo SG90 | 1 | $3 | Universal |
| LEDs 5mm (R/G/Y) | 3 | $1 | Universal |
| Buzzer Activo 5V | 1 | $2 | Universal |
| Resistencias 220Ω | 3 | $1 | Kit resistencias |

### Accesorios
| Accesorio | Cantidad | Precio | Notas |
|-----------|----------|--------|-------|
| Protoboard 830 puntos | 1 | $3 | Para prototipos |
| Cables Dupont M-M | 20 | $2 | Conexiones |
| Cables Dupont M-F | 10 | $2 | ESP32-CAM |
| Fuente 5V 2A | 1 | $5 | Alimentación |
| FTDI USB-Serial | 1 | $3 | Programar ESP32 |

**Total Hardware: ~$38 USD**

## 🔍 Verificación de Conexiones

### Checklist Pre-Encendido
- [ ] ✅ Arduino alimentado correctamente
- [ ] ✅ ESP32-CAM con 5V estable
- [ ] ✅ HC-SR04 VCC → 5V, GND → GND
- [ ] ✅ Servo rojo → 5V, marrón → GND
- [ ] ✅ LEDs con resistencias 220Ω
- [ ] ✅ Buzzer polaridad correcta
- [ ] ✅ Comunicación serial Arduino ↔ ESP32-CAM
- [ ] ✅ No cortocircuitos visibles

### Test Inicial
```arduino
// Código de prueba básico
void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH);
  Serial.println("LED ON");
  delay(1000);
  digitalWrite(13, LOW); 
  Serial.println("LED OFF");
  delay(1000);
}
```

## 🏠 Instalación Física

### Ubicación del Sensor
- 📏 **Altura**: 0.5-1m del suelo
- 📐 **Ángulo**: Perpendicular al suelo
- 🚗 **Distancia**: 2-5m antes de la tranquera
- 🌧️ **Protección**: Evitar lluvia directa

### Ubicación de la Cámara
- 📸 **Altura**: 1.5-2m del suelo
- 👁️ **Ángulo**: Enfocar placa frontal/trasera
- 💡 **Iluminación**: Evitar contraluz
- 🔒 **Seguridad**: Proteger de vandalismo

### Caja de Protección
- 🏠 **Material**: Plástico IP65 mínimo
- 🌡️ **Ventilación**: Evitar sobrecalentamiento
- 🔌 **Acceso**: Cables sellados
- 🔧 **Mantenimiento**: Acceso fácil

## ⚠️ Precauciones de Seguridad

### Eléctricas
- ⚡ Verificar polaridad antes de conectar
- 🔌 Usar fuente con protección cortocircuito
- 🧯 Tener extintor cerca (precaución)
- 👥 No trabajar solo con electricidad

### Mecánicas  
- 🔧 Servo con suficiente torque para tranquera
- ⚙️ Mecanismo de emergencia manual
- 🚨 Señalización del sistema automático
- 🛠️ Mantenimiento preventivo regular

---

💡 **Tip**: Documentar todas las conexiones con fotos antes del montaje final.