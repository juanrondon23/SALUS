#ifndef COMM_UART_H
#define COMM_UART_H

#include <Arduino.h>

void InicializaUart(long baud = 115200); //Funcion para llamar que inicializa comunicacion UART
void EnviarMensaje(const String& message); //Funcion para llamar que envia mensaje por UART (STRING)
bool RecibirMensaje(String& receivedMessage); //Funcion para recibir mensajes por UART (STRING)

#endif
