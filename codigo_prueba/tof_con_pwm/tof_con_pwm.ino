#include <Wire.h>
#include <VL53L0X.h>

// ---------------- PWM / SSR ----------------
const int pinRele = 2;                      // Control del SSR-40 DD
int dutyPorcentaje = 0;                     // 0 a 100 %
const unsigned long periodoTotalMs = 20;    // 50 Hz

// ---------------- Tacómetro ToF ----------------
VL53L0X sensor;
const int NUM_AGUJEROS = 8;

const float DISTANCIA_MIN = 10.0;           // mm
const float DISTANCIA_MAX = 100.0;          // mm

const unsigned long TIEMPO_MIN_PULSO = 2000; // µs, anti-rebote
unsigned long ultimoPulso = 0;
unsigned long intervaloPulso = 0;
bool estadoAnterior = false;
float rpmActual = 0;

void setup() {
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);

  Serial.begin(115200);
  while (!Serial && millis() < 3000) { ; }

  Wire.begin();
  Wire.setClock(100000);                    // I2C rápido para reducir el tiempo de lectura

  sensor.setTimeout(100);
  if (!sensor.init()) {
    Serial.println("Error: VL53L0X no detectado");
    while (1);
  }

  sensor.setMeasurementTimingBudget(10000); // 10 ms por medición
  sensor.startContinuous(0);

  Serial.println("Control PWM + Tacometro ToF iniciado");
  Serial.println("Presiona 'a' para AUMENTAR el ancho del pulso.");
  Serial.println("Presiona 'z' para REDUCIR el ancho del pulso.");
}

void loop() {
  // 1. Comandos por serial para el duty
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

  // 2. Generación del PWM (se ejecuta en cada vuelta del loop)
  unsigned long tiempoHigh = (periodoTotalMs * dutyPorcentaje) / 100;
  unsigned long cicloActual = millis() % periodoTotalMs;

  if (cicloActual < tiempoHigh && dutyPorcentaje > 0) {
    digitalWrite(pinRele, HIGH);
  } else {
    digitalWrite(pinRele, LOW);
  }

  // 3. Tacómetro: solo lee si el sensor ya tiene una medición lista (no bloqueante)
  if ((sensor.readReg(VL53L0X::RESULT_INTERRUPT_STATUS) & 0x07) != 0) {
    uint16_t distancia = sensor.readRangeContinuousMillimeters();

    bool viendoDisco = (distancia >= DISTANCIA_MIN && distancia <= DISTANCIA_MAX);
    bool agujero = !viendoDisco;

    if (agujero && !estadoAnterior) {
      unsigned long tiempoActual = micros();
      if (ultimoPulso != 0) {
        intervaloPulso = tiempoActual - ultimoPulso;
        if (intervaloPulso > TIEMPO_MIN_PULSO) {
          rpmActual = 60000000.0 / ((float)intervaloPulso * NUM_AGUJEROS);

          Serial.print("Duty: ");
          Serial.print(dutyPorcentaje);
          Serial.print(" % | RPM detectadas: ");
          Serial.println(rpmActual, 2);
        }
      }
      ultimoPulso = tiempoActual;
    }
    estadoAnterior = agujero;
  }
}