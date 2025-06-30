#include "../include/LibreriaCuatri.h"
#include <ArduinoOTA.h>
#include <WiFi.h>

static constexpr uint16_t  PulsoMinimoPix = -100;  // pulso mínimo Pixhawk
static constexpr uint16_t  PulsoMaximoPix = 100;  // pulso máximo Pixhawk
static constexpr uint8_t   DutyMinimo     = 40;    // duty mínimo que admite tu ESC
static constexpr uint8_t   DutyMaximo    = 150;   // duty máximo
static constexpr uint8_t   ZonaMuerta = 40;    // ~±40 µs ≈ ±2 % de zona muerta

// UART0 → ya es Serial (GPIO1 = TX0, GPIO3 = RX0)

void InicializaUart(long baud) {
  Serial.begin(baud);  // UART0 ya está mapeada a GPIO1 y GPIO3
}

void EnviarMensaje(const String& mensaje) {
  Serial.println(mensaje);  // Envía texto seguido de '\n'
}

bool RecibirMensaje(String& mensajerecibido) {
  if (Serial.available()) {
    mensajerecibido = Serial.readStringUntil('\n');
    return true;
  }
  return false;
}
// Inicializa WiFi para OTA
void InicializaWiFi(const char* ssid, const char* contraseña) {
  WiFi.begin(ssid, contraseña);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// Inicializa funcionalidad OTA
void InicializaOTA() {
  ArduinoOTA.setHostname("cuatri-esp32");

  ArduinoOTA.onStart([]() {
    String type = ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem";
    Serial.println("OTA Start: " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progreso: %u%%\r", (progress * 100) / total);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Fallo autenticación");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Fallo inicio");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Fallo conexión");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Fallo recepción");
    else if (error == OTA_END_ERROR) Serial.println("Fallo al finalizar");
  });

  ArduinoOTA.begin();
}
void InicializaParametrosAceleracion()
{
    pinMode(CH2, INPUT);
    pinMode(ACCEL_PWM, OUTPUT);
    // Configuración PWM ESP32
    ledcAttachPin(ACCEL_PWM, 0); // Pin, canal 0
    ledcSetup(0, 50, 8);         // Canal 0, 50 Hz, 8 bits
    ledcWrite(0, 0);             // Duty inicial en 0
}
/* Lee canal 2 (Pixhawk) y actualiza el duty de ACCEL_PWM */
void AceleradorConPixhawk()
{
    /* Lee un pulso de hasta 25 ms; 0 → sin señal */
    uint32_t pulse = pulseIn(CH2, HIGH, 25000);

    /* Si no hay señal → motor a reposo */
    if (pulse == 0) {
        ledcWrite(0, 0);
        return;
    }

    /* Aplica zona muerta (± ZonaMuerta) */
    if (pulse < PulsoMinimoPix + ZonaMuerta) {
        ledcWrite(0, 0);
        return;
    }

    /* Constrain + map: 1000-2000 µs → DutyMinimo-DutyMaximo */
    pulse = constrain(pulse, PulsoMinimoPix, PulsoMaximoPix);
    uint16_t duty = map(pulse, PulsoMinimoPix, PulsoMaximoPix, DutyMinimo, DutyMaximo);

    ledcWrite(0, duty);
}