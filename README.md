# ESP32 Sensor & Servo Control System

Sistema modular para control de sensores (DS18B20, MQ135) y servomotor MG996R con interfaz web.

**Proyecto Integrador 2 - Ingeniería en Sistemas Espaciales**  
Team Pipinas: Ailin Ferrari, Emiliano Vitale, Ezequiel Maceda, Johanna Olivera

## 📁 Estructura del Proyecto

```
ESP32_SensorControl/
├── ESP32_SensorControl.ino    // Archivo principal (setup + loop)
├── Config.h                    // Configuración WiFi y constantes
├── Sensors.h                   // Declaraciones de sensores
├── Sensors.cpp                 // Implementación de sensores
├── ServoControl.h              // Declaraciones del servo
├── ServoControl.cpp            // Implementación del servo
├── WebServerManager.h          // Declaraciones del servidor web
├── WebServerManager.cpp        // Implementación del servidor
├── WebUI.h                     // HTML embebido
└── README.md                   // Este archivo
```

## 🚀 Instalación

### Arduino IDE

1. Coloca **todos los archivos** (.ino, .h, .cpp) en la misma carpeta
2. Abre `ESP32_SensorControl.ino` con Arduino IDE
3. Modifica `Config.h` con tu red WiFi:
   ```cpp
   const char* WIFI_SSID = "TuRedWiFi";
   const char* WIFI_PASS = "TuContraseña";
   ```
4. Instala las librerías requeridas (ver abajo)
5. Selecciona tu placa ESP32
6. Compila y sube

### PlatformIO (Recomendado para proyectos profesionales)

```ini
[platformio]
src_dir = .

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    paulstoffregen/OneWire
    milesburton/DallasTemperature
    madhephaestus/ESP32Servo
```

## 📚 Librerías Requeridas

- **OneWire** by Paul Stoffregen
- **DallasTemperature** by Miles Burton
- **ESP32Servo** by Kevin Harrington

Instalar desde el Library Manager del Arduino IDE.

## 🔌 Conexiones Hardware

| Componente | Pin ESP32 | Descripción |
|------------|-----------|-------------|
| DS18B20 (DATA) | GPIO 4 | Sensor de temperatura |
| MQ135 (AOUT) | GPIO 34 | Sensor de gases (ADC) |
| MG996R (PWM) | GPIO 22 | Servomotor |

## ⚙️ Configuración

Edita `Config.h` para cambiar:

- **WiFi**: SSID y contraseña
- **Pines**: Si usas otros GPIO
- **Servo**: Límites de ángulo, velocidad
- **Sensores**: Resolución del DS18B20

## 🌐 Acceso Web

Después de subir el código:

1. Abre el **Serial Monitor** (115200 baud)
2. Busca la IP asignada (ej: `192.168.1.100`)
3. Accede desde tu navegador:
   - `http://192.168.1.100`
   - `http://esp-demo.local` (si mDNS funciona)

## 📡 API REST Endpoints

| Endpoint | Método | Descripción |
|----------|--------|-------------|
| `/` | GET | Interfaz web principal |
| `/temp` | GET | Lectura de temperatura |
| `/gas` | GET | Lectura del sensor de gas |
| `/start?speed=X` | GET | Iniciar barrido del servo |
| `/stop` | GET | Detener servo |
| `/set?angle=X` | GET | Mover servo a ángulo X |
| `/status` | GET | Estado del servo |

**Ejemplo de respuesta JSON:**
```json
{
  "ok": true,
  "temperature": 23.5
}
```

## 🎯 Ventajas de la Estructura Modular

### ✅ **Mantenibilidad**
- Cada módulo tiene una responsabilidad clara
- Fácil encontrar y corregir bugs

### ✅ **Escalabilidad**
- Agregar nuevos sensores: solo edita `Sensors.h/.cpp`
- Agregar endpoints: solo edita `WebServerManager.cpp`

### ✅ **Reutilización**
- Las clases pueden usarse en otros proyectos
- Fácil convertir en librerías

### ✅ **Colaboración**
- Múltiples personas pueden trabajar en paralelo
- Menos conflictos en Git

### ✅ **Testing**
- Cada módulo puede probarse independientemente
- Facilita unit testing

## 🔧 Personalización

### Agregar un nuevo sensor

1. Edita `Sensors.h` y agrega el método:
   ```cpp
   void readNewSensor(int& value);
   ```

2. Implementa en `Sensors.cpp`:
   ```cpp
   void SensorManager::readNewSensor(int& value) {
     value = analogRead(NEW_SENSOR_PIN);
   }
   ```

3. Agrega el endpoint en `WebServerManager.cpp`

### Cambiar interfaz web

Edita `WebUI.h` - el HTML está en la constante `MAIN_page`

## 📊 Optimizaciones Implementadas

- ✅ Cache de lecturas del DS18B20 (evita lecturas duplicadas)
- ✅ Resolución reducida a 10 bits (4x más rápido)
- ✅ Buffers estáticos (sin fragmentación de memoria)
- ✅ Precálculo de intervalos del servo
- ✅ HTML minificado
- ✅ Conversión no bloqueante de temperatura

## 🐛 Troubleshooting

**No conecta a WiFi:**
- Verifica SSID y contraseña en `Config.h`
- Prueba con otra red 2.4GHz (ESP32 no soporta 5GHz)

**Sensor de temperatura devuelve -127°C:**
- Verifica conexión del DS18B20
- Revisa resistencia pull-up de 4.7kΩ

**Servo no se mueve:**
- Verifica alimentación externa (el USB no es suficiente)
- Revisa conexión al GPIO 22

## 📝 Notas de Desarrollo

Este código sigue prácticas profesionales de desarrollo embebido:

- **Separación de responsabilidades** (Single Responsibility Principle)
- **Encapsulación** mediante clases
- **Configuración centralizada** en un solo archivo
- **Guardas de inclusión** para evitar duplicados
- **Nomenclatura clara** y consistente

## 📄 Licencia

Proyecto académico - Uso educativo libre

---

**¿Preguntas?** Contacta al Team Pipinas 🚀