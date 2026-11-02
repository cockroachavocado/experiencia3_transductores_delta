const int pinRele = 2;       // Pin conectado a la entrada de control del SSR-40 DD
int dutyPorcentaje = 0;      // Porcentaje de ancho de pulso (0 a 100%)

// Definimos el periodo total del PWM en milisegundos.
// 20 ms = 50 Hz (un periodo lento y de pulso ancho ideal para que el SSR y el motor respondan).
const unsigned long periodoTotalMs = 20; 

unsigned long tiempoAnterior = 0;

void setup() {
  pinMode(pinRele, OUTPUT);
  Serial.begin(9600);
  
  while (!Serial && millis() < 3000) {
    ;
  }

  Serial.println("Control de Ancho de Pulso (Periodo Largo) por Serial:");
  Serial.println("Presiona 'a' para AUMENTAR el ancho del pulso.");
  Serial.println("Presiona 'z' para REDUCIR el ancho del pulso.");
}

void loop() {
  // 1. Revisar si hay comandos en el puerto serie para cambiar el duty
  if (Serial.available() > 0) {
    char tecla = Serial.read();

    if (tecla == 'a' || tecla == 'A') {
      dutyPorcentaje += 5; // Sube 5% el ancho de pulso
      if (dutyPorcentaje > 100) dutyPorcentaje = 100;
      Serial.print("Ancho de pulso ajustado a: ");
      Serial.print(dutyPorcentaje);
      Serial.println(" %");
    } 
    else if (tecla == 'z' || tecla == 'Z') {
      dutyPorcentaje -= 5; // Baja 5% el ancho de pulso
      if (dutyPorcentaje < 0) dutyPorcentaje = 0;
      Serial.print("Ancho de pulso ajustado a: ");
      Serial.print(dutyPorcentaje);
      Serial.println(" %");
    }
  }

  // 2. Generar el tren de pulsos con el periodo largo especificado
  // Calculamos el tiempo que el pin estará en HIGH (tiempo de encendido dentro del periodo)
  unsigned long tiempoHigh = (periodoTotalMs * dutyPorcentaje) / 100;

  unsigned long tiempoActual = millis();
  unsigned long cicloActual = tiempoActual % periodoTotalMs;

  // Si estamos dentro de la porción de tiempo en HIGH del periodo actual
  if (cicloActual < tiempoHigh && dutyPorcentaje > 0) {
    digitalWrite(pinRele, HIGH);
  } else {
    digitalWrite(pinRele, LOW);
  }
}