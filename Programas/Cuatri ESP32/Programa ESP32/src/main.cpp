#include <Arduino.h>
#include "../include/LibreriaCuatri.h"
#include <ArduinoOTA.h>
#include <WiFi.h>

//defino la conexion a WiFi
const char* ssid = "JuanRondon";
const char* contrasena = "Manuela23";

void setup() {
  InicializaUart();  // Inicializa UART0 con 115200 baudios

  InicializaWiFi(ssid, contrasena); //hago el llamado desde el main para que se conecte al WiFi

  // Espera a la conexión WiFi y envía mensaje según el resultado
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    intentos++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    EnviarMensaje("Conexión WiFi exitosa: " + String(WiFi.localIP()));
  } else {
    EnviarMensaje("Error al conectar a WiFi");
  }

  InicializaOTA();
  InicializaParametrosAceleracion();          // deja pines listos
  delay(1000);
  EnviarMensaje("ESP32 conectado a Raspberry Pi");
}

void loop() {
  String mensaje;
  if (RecibirMensaje(mensaje)) {
    // Procesar el mensaje recibido
    EnviarMensaje("Mensaje recibido: " + mensaje);
  }
  AceleradorConPixhawk(); //llama a la funcion que lee el PWM del Pixhawk y lo envia al ESC
  ArduinoOTA.handle(); //Importante para el funcionamiento del OTA llama a la ESP a revisar el estado 
  delay(30);
}

