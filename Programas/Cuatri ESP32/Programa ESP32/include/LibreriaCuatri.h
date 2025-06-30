#ifndef LIBRERIA_CUATRI_H
#define LIBRERIA_CUATRI_H

#include <Arduino.h>

#define CH2          4          // entrada PWM proveniente del Pixhawk
#define ACCEL_PWM    17         // salida PWM hacia el ESC o driver

void InicializaUart(long baud = 115200); //Funcion para llamar que inicializa comunicacion UART
void EnviarMensaje(const String& mensaje); //Funcion para llamar que envia mensaje por UART (STRING)
bool RecibirMensaje(String& mensajerecibido); //Funcion para recibir mensajes por UART (STRING)
void InicializaWiFi(const char* ssid, const char* contraseña); //Funcion que inicia la conexion con la ESP por WiFi
void InicializaOTA(); //Funcion que inicializa OTA
void  InicializaParametrosAceleracion();     // configura pines y duty cero
void  AceleradorConPixhawk();       // debe llamarse en cada loop()
#endif
