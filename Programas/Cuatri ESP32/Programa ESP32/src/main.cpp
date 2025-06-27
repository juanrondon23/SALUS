#include <Arduino.h>
#include "../include/LibreriaCuatri.h"
void setup() {
  InicializaUart();  // Inicializa UART0 con 115200 baudios
  delay(1000);
  EnviarMensaje("ESP32 conectado a Raspberry Pi");
}

void loop() {
  String mensaje;
  if (RecibirMensaje(mensaje)) {
    // Procesar el mensaje recibido
    EnviarMensaje("Mensaje recibido: " + mensaje);
  }

  delay(1000);
}
