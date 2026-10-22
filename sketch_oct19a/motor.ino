const int pinRele = 9;
const int pinPot  = A0;

const unsigned long PERIODO_MS = 1000;   // un ciclo completo dura 1 segundo

void setup() {
  pinMode(pinRele, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int lectura = analogRead(pinPot);                         // 0 a 1023
  unsigned long tOn  = map(lectura, 0, 1023, 0, PERIODO_MS); // tiempo encendido
  unsigned long tOff = PERIODO_MS - tOn;                     // tiempo apagado

  if (tOn > 0) {
    digitalWrite(pinRele, HIGH);   // LED prende, motor gira
    delay(tOn);
  }
  if (tOff > 0) {
    digitalWrite(pinRele, LOW);    // LED apaga, motor para
    delay(tOff);
  }

  Serial.print("Duty: ");
  Serial.print(tOn * 100 / PERIODO_MS);
  Serial.println(" %");
}
