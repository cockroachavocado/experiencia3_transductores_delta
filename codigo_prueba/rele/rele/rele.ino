
const int pinRele = 2;

void setup() {
  digitalWrite(pinRele, LOW);
  pinMode(pinRele, OUTPUT);

  Serial.begin(9600);
  Serial.println("Prueba de control del rele");
}

void loop() {
  digitalWrite(pinRele, HIGH);
  Serial.println("Orden: ENCENDER");
  delay(3000);

  digitalWrite(pinRele, LOW);
  Serial.println("Orden: APAGAR");
  delay(3000);
}
