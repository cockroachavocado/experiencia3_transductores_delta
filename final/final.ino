#include <Wire.h>
#include <VL53L0X.h>

// NOTA: hace falta probar el inductivo









// ---------------- PWM / SSR ----------------
const int pinRele = 2;
int dutyPorcentaje = 0;
const unsigned long periodoTotalMs = 20;       // 50 Hz

// ---------------- Hall US1881 (interrupción) ----------------
const int pinHall = 3;
const int PULSOS_POR_REV_HALL = 2;             // nº total de imanes (polaridades alternadas N/S)
const unsigned long TIEMPO_MIN_HALL = 500;     // µs

volatile unsigned long ultimoHall = 0;
volatile unsigned long intervaloHall = 0;

// ---------------- Inductivo SN04-N (sondeo) ----------------
const int pinInductivo = 4;
const int PULSOS_POR_REV_IND = 1;              // piezas metálicas por vuelta
const unsigned long TIEMPO_MIN_IND = 1000;     // µs

unsigned long ultimoInd = 0;
unsigned long intervaloInd = 0;
int estadoAnteriorInd;

// ---------------- Tacómetro ToF ----------------
VL53L0X sensor;
const int NUM_AGUJEROS = 8;
const float DISTANCIA_MIN = 10.0;              // mm
const float DISTANCIA_MAX = 100.0;             // mm
const unsigned long TIEMPO_MIN_PULSO = 2000;   // µs

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

  pinMode(pinInductivo, INPUT_PULLUP);
  estadoAnteriorInd = digitalRead(pinInductivo);

  Serial.begin(115200);
  while (!Serial && millis() < 3000) { ; }

  Wire.begin();
  Wire.setClock(50000);

  sensor.setTimeout(100);
  if (!sensor.init()) {
    Serial.println("Error: VL53L0X no detectado");
    while (1);
  }
  sensor.setMeasurementTimingBudget(10000);
  sensor.startContinuous(0);

  Serial.println("Control PWM + Hall + Inductivo + Tacometro ToF");
  Serial.println("'a' = aumentar ancho de pulso | 'z' = reducir");
  Serial.print("Inductivo estado inicial: ");
  Serial.println(estadoAnteriorInd == LOW ? "METAL DETECTADO (LOW)" : "SIN METAL (HIGH)");
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

  // 3. Inductivo por sondeo (flanco HIGH -> LOW = metal detectado)
  int estadoActualInd = digitalRead(pinInductivo);
  if (estadoActualInd == LOW && estadoAnteriorInd == HIGH) {
    unsigned long ahora = micros();
    unsigned long dt = ahora - ultimoInd;
    if (dt > TIEMPO_MIN_IND) {
      if (ultimoInd != 0) intervaloInd = dt;
      ultimoInd = ahora;
    }
  }
  estadoAnteriorInd = estadoActualInd;

  // 4. ToF (no bloqueante: solo lee si hay medición lista)
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

  // 5. Reporte cada 500 ms
  if (millis() - ultimoPrint >= 500) {
    ultimoPrint = millis();

    noInterrupts();
    unsigned long intervaloH = intervaloHall;
    unsigned long ultimoH = ultimoHall;
    interrupts();

    unsigned long ahora = micros();

    float rpmHall = 0;
    if (intervaloH > 0 && (ahora - ultimoH) < 1000000UL) {
      rpmHall = 60000000.0 / ((float)intervaloH * PULSOS_POR_REV_HALL);
    }

    float rpmInd = 0;
    if (intervaloInd > 0 && (ahora - ultimoInd) < 1000000UL) {
      rpmInd = 60000000.0 / ((float)intervaloInd * PULSOS_POR_REV_IND);
    }

    float rpmToF = 0;
    if (intervaloToF > 0 && (ahora - ultimoPulsoToF) < 1000000UL) {
      rpmToF = 60000000.0 / ((float)intervaloToF * NUM_AGUJEROS);
    }

    Serial.print("Duty: ");
    Serial.print(dutyPorcentaje);
    Serial.print(" % | Hall: ");
    Serial.print(rpmHall, 1);
    Serial.print(" | Inductivo: ");
    Serial.print(rpmInd, 1);
    Serial.print(" | ToF: ");
    Serial.print(rpmToF, 1);
    Serial.println(" RPM");
  }
}