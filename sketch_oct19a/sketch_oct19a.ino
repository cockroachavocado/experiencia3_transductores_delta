// Pin del relé
int pinRele = 2;

void setup() {
  pinMode(pinRele, OUTPUT);
}

void loop() {
  digitalWrite(pinRele, HIGH);
  delay(1000); // Esperar 1 segundo
  digitalWrite(pinRele, LOW);
  delay(1000); // Esperar 1 segundo
}
