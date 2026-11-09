// ---------------- PWM / SSR ----------------
const int pinRele = 2;
int dutyPorcentaje = 0;
const unsigned long periodoTotalMs = 20;   // 50 Hz

// ---------------- Hall US1881 ----------------
const int pinHall = 3;
const int PULSOS_POR_REV = 2;              // cambios de estado por vuelta = nº total de imanes (alternando N/S)
const unsigned long TIEMPO_MIN_HALL = 500; // µs, anti-rebote

volatile unsigned long ultimoHall = 0;
volatile unsigned long intervaloHall = 0;
volatile bool hayPulsoHall = false;

unsigned long ultimoPrint = 0;

void isrHall() {
  unsigned long ahora = micros();
  unsigned long dt = ahora - ultimoHall;
  if (dt > TIEMPO_MIN_HALL) {
    if (ultimoHall != 0) {
      intervaloHall = dt;
      hayPulsoHall = true;
    }
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

  Serial.println("Control PWM + Sensor Hall US1881");
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

  // 3. RPM del Hall, reportadas cada 500 ms
  if (millis() - ultimoPrint >= 500) {
    ultimoPrint = millis();

    noInterrupts();
    unsigned long intervalo = intervaloHall;
    unsigned long ultimo = ultimoHall;
    interrupts();

    float rpm = 0;
    // Si pasó más de 1 s sin pulsos, se considera detenido
    if (intervalo > 0 && (micros() - ultimo) < 1000000UL) {
      rpm = 60000000.0 / ((float)intervalo * PULSOS_POR_REV);
    }

    Serial.print("Duty: ");
    Serial.print(dutyPorcentaje);
    Serial.print(" % | Hall RPM: ");
    Serial.println(rpm, 1);
  }
}