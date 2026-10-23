
const byte pinHall = 3;

const float pulsosPorVuelta = 1.0;
const unsigned long intervalo = 1000;

volatile unsigned long pulsos = 0;

unsigned long tiempoAnterior = 0;

void contarPulso() {
  pulsos++;
}

void setup() {
  Serial.begin(9600);

  pinMode(pinHall, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(pinHall),
    contarPulso,
    FALLING
  );

  tiempoAnterior = millis();
  Serial.println("Medicion de RPM");
}

void loop() {
  unsigned long ahora = millis();

  if (ahora - tiempoAnterior >= intervalo) {

    unsigned long transcurrido =
      ahora - tiempoAnterior;

    noInterrupts();

    unsigned long cantidad = pulsos;
    pulsos = 0;

    interrupts();

    float rpm =
      (cantidad * 60000.0) /
      (pulsosPorVuelta * transcurrido);

    Serial.print("Pulsos: ");
    Serial.print(cantidad);

    Serial.print(" | RPM: ");
    Serial.println(rpm, 1);

    tiempoAnterior = ahora;
  }
}
