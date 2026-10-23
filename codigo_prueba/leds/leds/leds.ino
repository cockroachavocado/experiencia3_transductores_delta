
const int ledVerde = 8;
const int ledRojo = 9;
const int ledAzul = 10;

void setup() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledRojo, OUTPUT);
  pinMode(ledAzul, OUTPUT);
}

void loop() {
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledRojo, LOW);
  digitalWrite(ledAzul, LOW);
  delay(1000);

  digitalWrite(ledVerde, LOW);
  digitalWrite(ledRojo, HIGH);
  digitalWrite(ledAzul, LOW);
  delay(1000);

  digitalWrite(ledVerde, LOW);
  digitalWrite(ledRojo, LOW);
  digitalWrite(ledAzul, HIGH);
  delay(1000);
}
