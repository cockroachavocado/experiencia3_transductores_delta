#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;
const int NUM_AGUJEROS = 8;

const int DISTANCIA_MIN = 9.5; //rango de distancia entre el sensor y el disco
const int DISTANCIA_MAX = 11.0;

const unsigned long TIEMPO_MIN_PULSO = 2000;
unsigned long ultimoPulso = 0;
unsigned long intervaloPulso = 0;
bool estadoAnterior = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sensor.setTimeout(100);

  if (!sensor.init()) {
    Serial.println("Error: VL53L0X no detectado");
    while (1);
  }

  sensor.setMeasurementTimingBudget(20000);
  sensor.startContinuous(0);
  Serial.println("Tacometro iniciado");
}

void loop() {
  int distancia = sensor.readRangeContinuousMillimeters();
  if (sensor.timeoutOccurred()) {
    return;
  }
  bool viendoDisco =
      (distancia >= DISTANCIA_MIN &&
       distancia <= DISTANCIA_MAX);

  bool agujero = !viendoDisco;
  if (agujero && !estadoAnterior) {
    unsigned long tiempoActual = micros();
    if (ultimoPulso != 0) {
      intervaloPulso = tiempoActual - ultimoPulso;
      if (intervaloPulso > TIEMPO_MIN_PULSO) {
        float rpm =
          (60000000.0 /
          (intervaloPulso * NUM_AGUJEROS));

        Serial.print("RPM: ");
        Serial.println(rpm, 2);
      }
    }
    ultimoPulso = tiempoActual;
  }

  estadoAnterior = agujero;
}


