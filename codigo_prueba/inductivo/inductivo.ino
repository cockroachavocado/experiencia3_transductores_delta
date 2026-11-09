const int pinInductivo = 4;

int estadoAnterior;

void setup() {
  Serial.begin(9600);

  pinMode(pinInductivo, INPUT_PULLUP);

  estadoAnterior = digitalRead(pinInductivo);

  Serial.println("Prueba sensor inductivo SN04-N");
  Serial.println("Acerca y aleja una pieza metalica");
  Serial.print("Estado inicial: ");
  Serial.println(estadoAnterior == LOW ? "METAL DETECTADO (LOW)" : "SIN METAL (HIGH)");
}

void loop() {
  int estadoActual = digitalRead(pinInductivo);

  if (estadoActual != estadoAnterior) {
    if (estadoActual == LOW) {
      Serial.println("Inductivo: LOW (metal detectado)");
    } else {
      Serial.println("Inductivo: HIGH (sin metal)");
    }

    estadoAnterior = estadoActual;
  }
}