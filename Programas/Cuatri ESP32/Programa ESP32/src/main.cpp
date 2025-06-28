#include <Arduino.h>
#include "../include/LibreriaCuatri.h"
#include <ArduinoOTA.h>
#include <WiFi.h>

//defino la conexion a WiFi
const char* ssid=("Aca va el ssid del WiFi");
const char* contraseña=("aca va la contraseña del WiFi");
void setup() {
  InicializaUart();  // Inicializa UART0 con 115200 baudios
  InicializaWiFi(ssid,contraseña); //hago el llamado desde el main para que se conecte al WiFi
  InicializaOTA();
  delay(1000);
  EnviarMensaje("ESP32 conectado a Raspberry Pi");
}

void loop() {
  String mensaje;
  if (RecibirMensaje(mensaje)) {
    // Procesar el mensaje recibido
    EnviarMensaje("Mensaje recibido: " + mensaje);
  }
  ArduinoOTA.handle(); //Importante para el funcionamiento del OTA llama a la ESP a revisar el estado 
  delay(1000);
}
