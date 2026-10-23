
const int pinPot = A0;

void setup() {
  Serial.begin(9600);
  pinMode(pinPot, INPUT);
}

void loop() {
  int lectura = analogRead(pinPot);

  int rpmReferencia = map(lectura, 0, 1023, 0, 300);

  Serial.print("Potenciometro: ");
  Serial.print(lectura);

  Serial.print(" | Referencia: ");
  Serial.print(rpmReferencia);
  Serial.println(" RPM");

  delay(300);
}
