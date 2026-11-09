#include <Wire.h>
#include <VL53L0X.h>

// ---------------- PWM / SSR ----------------
const int pinRele = 2;
int dutyPorcentaje = 0;
const unsigned long periodoTotalMs = 20;

// ---------------- Hall US1881 ----------------
const int pinHall = 3;
const int PULSOS_POR_REV_HALL = 2;         // nº total de imanes (alternando N/S)
const unsigned long TIEMPO_MIN_HALL = 500; // µs

volatile unsigned long ultimoHall = 0;
volatile unsigned long intervaloHall = 0;

// ---------------- Tacómetro ToF ----------------
VL53L0X sensor;
const int NUM_AGUJEROS = 8;
const float DISTANCIA_MIN = 10.0;          // mm
const float DISTANCIA_MAX = 100.0;         // mm
const unsigned long TIEMPO_MIN_PULSO = 2000; // µs

unsigned long ultimoPulsoToF = 0;
unsigned long intervaloToF = 0;
bool estadoAnteriorToF = false;

unsigned long ultimoPrint = 0;

void isrHall() {
  unsigned long ahora = micros();
  unsigned long dt = ahora - ultimoHall;
  if (dt > TIEMPO_MIN_HALL) {
    if (ultimoHall != 0) intervaloHall = dt;
    ultimoHall = ahora;
  }
}

void setup() {
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);

  pinMode(pinHall, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinHall), isrHall, CHANGE);

  Serial.begin(115200);
  while (!Serial && millis() < 3000) { ; }

  Wire.begin();
  Wire.setClock(100000);

  sensor.setTimeout(100);
  if (!sensor.init()) {
    Serial.println("Error: VL53L0X no detectado");
    while (1);
  }
  sensor.setMeasurementTimingBudget(10000);
  sensor.startContinuous(0);

  Serial.println("Control PWM + Hall + Tacometro ToF");
  Serial.println("'a' = aumentar ancho de pulso | 'z' = reducir");
}

void loop() {
  // 1. Comandos serial
  if (Serial.available() > 0) {
    char tecla = Serial.read();
    if (tecla == 'a' || tecla == 'A') {
      dutyPorcentaje += 5;
      if (dutyPorcentaje > 100) dutyPorcentaje = 100;
      Serial.print("Ancho de pulso ajustado a: ");
      Serial.print(dutyPorcentaje);
      Serial.println(" %");
    }
    else if (tecla == 'z' || tecla == 'Z') {
      dutyPorcentaje -= 5;
      if (dutyPorcentaje < 0) dutyPorcentaje = 0;
      Serial.print("Ancho de pulso ajustado a: ");
      Serial.print(dutyPorcentaje);
      Serial.println(" %");
    }
  }

  // 2. PWM
  unsigned long tiempoHigh = (periodoTotalMs * dutyPorcentaje) / 100;
  unsigned long cicloActual = millis() % periodoTotalMs;
  if (cicloActual < tiempoHigh && dutyPorcentaje > 0) {
    digitalWrite(pinRele, HIGH);
  } else {
    digitalWrite(pinRele, LOW);
  }

  // 3. ToF (no bloqueante: solo lee si hay medición lista)
  if ((sensor.readReg(VL53L0X::RESULT_INTERRUPT_STATUS) & 0x07) != 0) {
    uint16_t distancia = sensor.readRangeContinuousMillimeters();

    bool viendoDisco = (distancia >= DISTANCIA_MIN && distancia <= DISTANCIA_MAX);
    bool agujero = !viendoDisco;

    if (agujero && !estadoAnteriorToF) {
      unsigned long ahora = micros();
      if (ultimoPulsoToF != 0) {
        unsigned long dt = ahora - ultimoPulsoToF;
        if (dt > TIEMPO_MIN_PULSO) intervaloToF = dt;
      }
      ultimoPulsoToF = ahora;
    }
    estadoAnteriorToF = agujero;
  }

  // 4. Reporte cada 500 ms
  if (millis() - ultimoPrint >= 500) {
    ultimoPrint = millis();

    noInterrupts();
    unsigned long intervaloH = intervaloHall;
    unsigned long ultimoH = ultimoHall;
    interrupts();

    float rpmHall = 0;
    if (intervaloH > 0 && (micros() - ultimoH) < 1000000UL) {
      rpmHall = 60000000.0 / ((float)intervaloH * PULSOS_POR_REV_HALL);
    }

    float rpmToF = 0;
    if (intervaloToF > 0 && (micros() - ultimoPulsoToF) < 1000000UL) {
      rpmToF = 60000000.0 / ((float)intervaloToF * NUM_AGUJEROS);
    }

    Serial.print("Duty: ");
    Serial.print(dutyPorcentaje);
    Serial.print(" % | Hall RPM: ");
    Serial.print(rpmHall, 1);
    Serial.print(" | ToF RPM: ");
    Serial.println(rpmToF, 1);
  }
}