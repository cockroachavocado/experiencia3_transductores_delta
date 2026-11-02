const int pinRele = 9;   
const int pinPot  = A0;   

void setup() {
  pinMode(pinRele, OUTPUT);
  Serial.begin(9600);

  // Espera a que el Monitor Serie USB se active
  while (!Serial && millis() < 3000) {
    ;
  }
}

void loop() {
  int lectura = analogRead(pinPot);   
  int duty = lectura / 4;             // Escala de 0-1023 a 0-255
  
  analogWrite(pinRele, duty);         // PWM estándar en el pin 9

  Serial.print("Duty: ");
  Serial.print(duty * 100 / 255);
  Serial.println(" %");
  
  delay(100);
}