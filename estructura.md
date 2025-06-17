control-acceso/
├── README.md                    # Documentación principal
├── INSTALACION.md              # Guía de instalación
├── hardware/
│   ├── arduino/
│   │   ├── main.ino            # Código Arduino
│   │   └── config.h            # Configuración Arduino
│   └── esp32-cam/
│       ├── main.ino            # Código ESP32-CAM
│       └── config.h            # Configuración ESP32-CAM
├── backend/
│   ├── app.py                  # Backend Python
│   ├── requirements.txt        # Dependencias Python
│   ├── .env.example           # Plantilla variables de entorno
│   └── Dockerfile             # Para despliegue (opcional)
├── firebase/
│   ├── database-structure.json # Estructura de datos
│   ├── security-rules.json    # Reglas de seguridad
│   └── firebase-config.js     # Configuración web (opcional)
└── docs/
    ├── api.md                 # Documentación API
    ├── hardware-setup.md      # Guía conexiones
    └── troubleshooting.md     # Solución de problemas