#include "../include/LibreriaCuatri.h"
#include <ArduinoOTA.h>
#include <WiFi.h>

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
