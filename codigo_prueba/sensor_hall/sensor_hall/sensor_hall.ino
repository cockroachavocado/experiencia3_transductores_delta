
const int pinHall = 3;

int estadoAnterior;

void setup() {
  Serial.begin(9600);

  pinMode(pinHall, INPUT_PULLUP);

  estadoAnterior = digitalRead(pinHall);

  Serial.println("Prueba sensor Hall US1881");
  Serial.println("Probar ambos polos del iman");
}

void loop() {
  int estadoActual = digitalRead(pinHall);

  if (estadoActual != estadoAnterior) {
    if (estadoActual == LOW) {
      Serial.println("Hall: LOW");
    } else {
      Serial.println("Hall: HIGH");
    }

    estadoAnterior = estadoActual;
  }
}
