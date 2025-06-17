"""
SISTEMA DE CONTROL DE ACCESO VEHICULAR
Backend Python - API y OCR Avanzado
Archivo: control-acceso/backend/app.py
"""

from flask import Flask, request, jsonify
import cv2
import numpy as np
import pytesseract
import re
import base64
import io
from PIL import Image
import firebase_admin
from firebase_admin import credentials, db
import requests
import os
from datetime import datetime
import logging
from dotenv import load_dotenv

# Cargar variables de entorno
load_dotenv()

# Configuración de logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = Flask(__name__)

# Configuración desde variables de entorno
FIREBASE_CREDS_PATH = os.getenv('FIREBASE_CREDS_PATH', './firebase-credentials.json')
FIREBASE_DB_URL = os.getenv('FIREBASE_DB_URL')
TWILIO_ACCOUNT_SID = os.getenv('TWILIO_ACCOUNT_SID')
TWILIO_AUTH_TOKEN = os.getenv('TWILIO_AUTH_TOKEN')
TWILIO_PHONE_FROM = os.getenv('TWILIO_PHONE_FROM')
PHONE_SEGURIDAD = os.getenv('PHONE_SEGURIDAD')

# Inicializar Firebase
firebase_conectado = False
try:
    if os.path.exists(FIREBASE_CREDS_PATH):
        cred = credentials.Certificate(FIREBASE_CREDS_PATH)
        firebase_admin.initialize_app(cred, {'databaseURL': FIREBASE_DB_URL})
        firebase_conectado = True
        logger.info("✅ Firebase inicializado")
    else:
        logger.warning("⚠️ Credenciales Firebase no encontradas")
except Exception as e:
    logger.error(f"❌ Error Firebase: {e}")

class PlateRecognizer:
    def __init__(self):
        # Patrones de placas
        self.plate_patterns = [
            r'[A-Z]{3}[0-9]{3}',  # ABC123
            r'[A-Z]{2}[0-9]{4}',  # AB1234
        ]
    
    def extract_plate(self, image_data):
        """Extraer placa de imagen base64"""
        try:
            # Decodificar imagen
            if ',' in image_data:
                image_data = image_data.split(',')[1]
            
            image_bytes = base64.b64decode(image_data)
            image = Image.open(io.BytesIO(image_bytes))
            image_cv = cv2.cvtColor(np.array(image), cv2.COLOR_RGB2BGR)
            
            # Preprocesar
            processed = self.preprocess_image(image_cv)
            
            # OCR
            text = self.extract_text(processed)
            
            # Buscar patrón
            for pattern in self.plate_patterns:
                matches = re.findall(pattern, text)
                if matches:
                    return matches[0]
            
            # Fallback: secuencias alfanuméricas
            alphanumeric = re.findall(r'[A-Z0-9]{6,8}', text)
            if alphanumeric:
                return alphanumeric[0][:6]
            
            return ""
            
        except Exception as e:
            logger.error(f"Error procesando imagen: {e}")
            return ""
    
    def preprocess_image(self, image):
        """Preprocesar imagen para OCR"""
        # Escala de grises
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        
        # Filtro bilateral
        filtered = cv2.bilateralFilter(gray, 11, 17, 17)
        
        # Detectar bordes
        edges = cv2.Canny(filtered, 30, 200)
        
        return gray  # Retornar imagen procesada
    
    def extract_text(self, image):
        """Extraer texto con Tesseract"""
        try:
            config = '--oem 3 --psm 8 -c tessedit_char_whitelist=ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
            text = pytesseract.image_to_string(image, config=config)
            return text.strip().upper()
        except Exception as e:
            logger.error(f"Error OCR: {e}")
            return ""

class AccessControl:
    def __init__(self):
        self.plate_recognizer = PlateRecognizer()
        self.placas_locales = ["ABC123", "DEF456", "GHI789", "JKL012"]
    
    def verify_plate(self, plate):
        """Verificar placa en Firebase"""
        if not firebase_conectado:
            return self.verify_plate_local(plate)
        
        try:
            ref = db.reference(f'/residentes/{plate}')
            resident_data = ref.get()
            
            if resident_data:
                logger.info(f"✅ Placa autorizada: {plate}")
                self.log_access(plate, "autorizado")
                return {"authorized": True, "resident": resident_data}
            else:
                logger.info(f"❌ Placa no autorizada: {plate}")
                self.handle_unauthorized_vehicle(plate)
                return {"authorized": False, "plate": plate}
                
        except Exception as e:
            logger.error(f"Error Firebase: {e}")
            return self.verify_plate_local(plate)
    
    def verify_plate_local(self, plate):
        """Verificación local"""
        if plate in self.placas_locales:
            self.log_access_local(plate, "autorizado")
            return {"authorized": True, "resident": {"name": "Residente Local"}}
        else:
            self.handle_unauthorized_vehicle(plate)
            return {"authorized": False, "plate": plate}
    
    def log_access(self, plate, access_type):
        """Registrar acceso en Firebase"""
        if not firebase_conectado:
            return self.log_access_local(plate, access_type)
        
        try:
            ref = db.reference('/accesos')
            ref.push({
                'placa': plate,
                'tipo': access_type,
                'timestamp': datetime.now().isoformat(),
                'fecha': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                'ubicacion': 'entrada_principal',
                'procesado_por': 'python_backend'
            })
            logger.info(f"✅ Acceso registrado: {plate}")
        except Exception as e:
            logger.error(f"Error registrando acceso: {e}")
            self.log_access_local(plate, access_type)
    
    def log_access_local(self, plate, access_type):
        """Log local en caso de fallo Firebase"""
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        logger.info(f"📝 LOG LOCAL: {plate} - {access_type} - {timestamp}")
    
    def handle_unauthorized_vehicle(self, plate):
        """Manejar vehículo no autorizado"""
        self.log_temporary_vehicle(plate)
        self.send_security_alert(plate)
    
    def log_temporary_vehicle(self, plate):
        """Registrar vehículo temporal"""
        if not firebase_conectado:
            return
        
        try:
            ref = db.reference('/vehiculos_temporales')
            ref.push({
                'placa': plate,
                'timestamp': datetime.now().isoformat(),
                'fecha': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                'tipo': 'no_autorizado',
                'alerta_enviada': True,
                'procesado_por': 'python_backend'
            })
            logger.info(f"✅ Temporal registrado: {plate}")
        except Exception as e:
            logger.error(f"Error registrando temporal: {e}")
    
    def send_security_alert(self, plate):
        """Enviar SMS de alerta"""
        if not all([TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN, TWILIO_PHONE_FROM, PHONE_SEGURIDAD]):
            logger.warning("⚠️ Twilio no configurado")
            return
        
        try:
            url = f"https://api.twilio.com/2010-04-01/Accounts/{TWILIO_ACCOUNT_SID}/Messages.json"
            
            mensaje = f"""🚨 ALERTA SEGURIDAD 🚨
Vehículo NO autorizado
Placa: {plate}
Hora: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
Ubicación: Entrada Principal
Sistema: Backend Python"""
            
            data = {
                'To': PHONE_SEGURIDAD,
                'From': TWILIO_PHONE_FROM,
                'Body': mensaje
            }
            
            response = requests.post(url, data=data, auth=(TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN))
            
            if response.status_code == 201:
                logger.info("✅ SMS enviado")
            else:
                logger.error(f"❌ Error SMS: {response.status_code}")
                
        except Exception as e:
            logger.error(f"Error SMS: {e}")

# Instancia global
access_control = AccessControl()

@app.route('/health', methods=['GET'])
def health_check():
    """Estado del sistema"""
    return jsonify({
        'status': 'healthy',
        'timestamp': datetime.now().isoformat(),
        'services': {
            'firebase': 'connected' if firebase_conectado else 'disconnected',
            'twilio': 'configured' if TWILIO_ACCOUNT_SID else 'not_configured',
            'ocr': 'operational'
        }
    })

@app.route('/process_plate', methods=['POST'])
def process_plate():
    """Procesar imagen de placa"""
    try:
        data = request.get_json()
        
        if 'image' not in data:
            return jsonify({'error': 'No image provided'}), 400
        
        # Extraer placa
        plate = access_control.plate_recognizer.extract_plate(data['image'])
        
        if not plate:
            return jsonify({
                'success': False,
                'error': 'No plate detected'
            }), 400
        
        # Verificar autorización
        result = access_control.verify_plate(plate)
        
        return jsonify({
            'success': True,
            'plate': plate,
            'authorized': result['authorized'],
            'resident': result.get('resident'),
            'timestamp': datetime.now().isoformat()
        })
        
    except Exception as e:
        logger.error(f"Error procesando: {e}")
        return jsonify({'success': False, 'error': str(e)}), 500

@app.route('/verify_plate', methods=['POST'])
def verify_plate_only():
    """Verificar placa específica"""
    try:
        data = request.get_json()
        
        if 'plate' not in data:
            return jsonify({'error': 'No plate provided'}), 400
        
        plate = data['plate'].upper().strip()
        result = access_control.verify_plate(plate)
        
        return jsonify({
            'success': True,
            'plate': plate,
            'authorized': result['authorized'],
            'resident': result.get('resident'),
            'timestamp': datetime.now().isoformat()
        })
        
    except Exception as e:
        logger.error(f"Error verificando: {e}")
        return jsonify({'success': False, 'error': str(e)}), 500

@app.route('/add_resident', methods=['POST'])
def add_resident():
    """Agregar residente"""
    if not firebase_conectado:
        return jsonify({'error': 'Firebase not connected'}), 503
    
    try:
        data = request.get_json()
        required_fields = ['plate', 'name', 'apartment']
        
        if not all(field in data for field in required_fields):
            return jsonify({'error': 'Missing fields'}), 400
        
        plate = data['plate'].upper().strip()
        ref = db.reference(f'/residentes/{plate}')
        
        resident_data = {
            'nombre': data['name'],
            'departamento': data['apartment'],
            'telefono': data.get('phone', ''),
            'activo': True,
            'fecha_registro': datetime.now().isoformat()
        }
        
        ref.set(resident_data)
        logger.info(f"✅ Residente agregado: {plate}")
        
        return jsonify({
            'success': True,
            'plate': plate,
            'resident': resident_data
        })
        
    except Exception as e:
        logger.error(f"Error agregando residente: {e}")
        return jsonify({'success': False, 'error': str(e)}), 500

if __name__ == '__main__':
    print("========================================")
    print("🐍 BACKEND PYTHON - CONTROL DE ACCESO")
    print("========================================")
    print(f"🔥 Firebase: {'✅' if firebase_conectado else '❌'}")
    print(f"📱 Twilio: {'✅' if TWILIO_ACCOUNT_SID else '❌'}")
    print("🤖 OCR: Tesseract")
    print("🌐 API: Flask")
    print("========================================")
    
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port, debug=True)