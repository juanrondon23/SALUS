#include "../include/LibreriaCuatri.h"

// UART0 → ya es Serial (GPIO1 = TX0, GPIO3 = RX0)

void InicializaUart(long baud) {
  Serial.begin(baud);  // UART0 ya está mapeada a GPIO1 y GPIO3
}

void EnviarMensaje(const String& message) {
  Serial.println(message);  // Envía texto seguido de '\n'
}

bool RecibirMensaje(String& receivedMessage) {
  if (Serial.available()) {
    receivedMessage = Serial.readStringUntil('\n');
    return true;
  }
  return false;
}
