const int pinRele = 9;    
const int pinPot  = A0;   

void setup() {
  pinMode(pinRele, OUTPUT);
  TCCR1B = (TCCR1B & 0xF8) | 0x05;   
  Serial.begin(9600);
}

void loop() {
  int lectura = analogRead(pinPot);   
  int duty = lectura / 4;           
  analogWrite(pinRele, duty); //PWM hacia el relé

  Serial.print("Duty: ");
  Serial.print(duty * 100 / 255);
  Serial.println(" %");
  delay(100);
}
