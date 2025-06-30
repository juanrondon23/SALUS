#include "LibreriaCuatri.h"
#include <WiFi.h>
#include <ArduinoOTA.h>

/*─────── PWM / Pulsos ─────────────────────────*/
static constexpr uint16_t PULSE_MIN_US = 1000;   // –100 %
static constexpr uint16_t PULSE_NEU_US = 1500;   //    0 %
static constexpr uint16_t PULSE_MAX_US = 2000;   // +100 %
static constexpr uint16_t DZ_US        = 40;     // zona muerta ±2 %

static constexpr uint8_t  DUTY_MIN = 40;
static constexpr uint8_t  DUTY_MAX = 150;
/*──────────────────────────────────────────────*/

/*─────── Estado interno ───────────────────────*/
enum class Mode { NEU, FWD, REV_WAIT, REV_RUN };
static Mode    mode             = Mode::NEU;

static bool    relayCmdSent     = false;  // se envió RELAY_REV_ON
static bool    reverseReady     = false;  // llegó LISTO REVERSA
static String  rxBuffer;
/*──────────────────────────────────────────────*/

/* ========= UART ========= */
void InicializaUart(long baud) { Serial.begin(baud); }

void EnviarMensaje(const String& txt) { Serial.println(txt); }

bool RecibirMensaje(String& txt) {
    if (Serial.available()) { txt = Serial.readStringUntil('\n'); return true; }
    return false;
}

/* ========= Wi-Fi ========= */
void InicializaWiFi(const char* ssid, const char* pass) {
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) delay(300);
}

/* ========= OTA ========= */
void InicializaOTA() {
    ArduinoOTA.setHostname("cuatri-esp32");

    ArduinoOTA.onStart([]() {
        EnviarMensaje("*** OTA INICIO ***");
    });
    ArduinoOTA.onEnd([]() {
        EnviarMensaje("*** OTA FIN ***");
    });
    ArduinoOTA.onError([](ota_error_t e) {
        EnviarMensaje("Error OTA: " + String(e));
    });
    ArduinoOTA.begin();
}

/* ========= PWM & acelerador ========= */
void InicializaParametrosAceleracion() {
    pinMode(CH2, INPUT);
    ledcAttachPin(ACCEL_PWM, 0);      // canal 0
    ledcSetup(0, 50, 8);              // 50 Hz, 8 bits
    ledcWrite(0, 0);
}

/*––– procesa líneas UART que vengan de la RPi –––*/
static void uartPoll() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (rxBuffer.equalsIgnoreCase("LISTO REVERSA"))
                reverseReady = true;
            rxBuffer = "";
        } else {
            rxBuffer += c;
            if (rxBuffer.length() > 40) rxBuffer.remove(0);
        }
    }
}

/*––– mapea pulso → duty según límites –––*/
static uint8_t mapDuty(uint32_t p1, uint32_t p2, bool invert = false) {
    uint16_t duty = map(p1,
                        invert ? PULSE_MIN_US : PULSE_NEU_US + DZ_US,
                        invert ? PULSE_NEU_US - DZ_US : PULSE_MAX_US,
                        DUTY_MAX, DUTY_MIN);
    return constrain(duty, DUTY_MIN, DUTY_MAX);
}

void AceleradorConPixhawk() {
    uartPoll();                             // ① atiende UART

    uint32_t pulse = pulseIn(CH2, HIGH, 25000);

    /* sin señal: reset total */
    if (pulse == 0) {
        ledcWrite(0, 0);
        if (relayCmdSent) EnviarMensaje("RELAY_REV_OFF");
        mode           = Mode::NEU;
        relayCmdSent   = false;
        reverseReady   = false;
        return;
    }

    /* decidir dirección */
    if (pulse > PULSE_NEU_US + DZ_US) {                 // avance
        if (mode == Mode::REV_WAIT || mode == Mode::REV_RUN)
            EnviarMensaje("RELAY_REV_OFF");
        mode         = Mode::FWD;
        relayCmdSent = reverseReady = false;
    }
    else if (pulse < PULSE_NEU_US - DZ_US) {            // reversa
        if (!relayCmdSent) {                            // enviar una vez
            EnviarMensaje("RELAY_REV_ON");
            relayCmdSent = true;
            reverseReady = false;
        }
        mode = reverseReady ? Mode::REV_RUN : Mode::REV_WAIT;
    }
    else {                                              // neutro
        if (mode == Mode::REV_WAIT || mode == Mode::REV_RUN)
            EnviarMensaje("RELAY_REV_OFF");
        mode         = Mode::NEU;
        relayCmdSent = reverseReady = false;
    }

    /* aplicar PWM según estado */
    switch (mode) {

      case Mode::FWD:
          ledcWrite(0, mapDuty(pulse, PULSE_MAX_US));
          break;

      case Mode::REV_RUN:
          ledcWrite(0, mapDuty(pulse, PULSE_NEU_US, true)); // invertido
          break;

      default:                                           // NEU o REV_WAIT
          ledcWrite(0, 0);
          break;
    }
}
