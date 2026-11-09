## Mapa de conexiones

Placa: **Arduino UNO R4 Minima**

### Asignación de pines en el código

```cpp
const int pinRele = 2;        // Control del SSR-40 DD
const int pinHall = 3;        // Hall US1881 (interrupción)
const int pinInductivo = 4;   // Inductivo SN04-N (sondeo)
```

### Resumen por pin del Arduino

| Pin | Función |
|---|---|
| 2 | Control del SSR (salida) |
| 3 | Salida del Hall (interrupción) |
| 4 | Salida del inductivo (sondeo) |
| SDA | VL53L0X |
| SCL | VL53L0X |
| 5V | Alimentación del Hall y del VL53L0X |
| GND | Común de todos los componentes |

### Conexiones por componente

| Componente | Terminal | Se conecta a |
|---|---|---|
| **SSR-40 DD** | Entrada (+) | Pin **2** |
| | Entrada (-) | GND Arduino |
| | Salida positiva | En serie con el motor y la entrada positiva de la fuente (respetar polaridad) |
| | Salida negativa | Entrada negativa de la fuente |
| **Hall US1881** | VCC (pin 1) | 5V Arduino |
| | GND (pin 2) | GND Arduino |
| | Salida (pin 3) | Pin **3** |
| **Inductivo SN04-N** (falta probarlo) | Marrón (+) | Fuente externa de 6 a 36 V (+) |
| | Azul (-) | GND de la fuente **y** GND Arduino |
| | Negro (salida) | Pin **4** |
| **VL53L0X** | VCC | 5V Arduino |
| | GND | GND Arduino |
| | SDA | SDA Arduino |
| | SCL | SCL Arduino |

### Notas importantes

- **Verificar el TOF:** verificar que el SCL y el SDA del tof estén correctamente conectados a sus respectivas entradas en el Arduino, ya que si no se conectan bien, el Serial Monitor podría dar el error de que el tof no se detectó.
- **Masa común:** todos los GND deben estar unidos (Arduino, Hall, VL53L0X, fuente del inductivo y entrada (-) del SSR).
- **Inductivo NPN:** la salida (cable negro) solo se conecta al pin 4. No se le agrega resistencia ni alimentación externa, porque el `INPUT_PULLUP` interno la sube a 5 V. Si el sensor fuera **PNP**, la salida entregaría el voltaje de alimentación y dañaría el pin; en ese caso se necesita un divisor de voltaje o un optoacoplador.
- **Hall US1881:** es bipolar con latch, por lo que el disco necesita imanes con polaridades alternadas (N/S). Si las lecturas son inestables, añadir una resistencia de 4.7 kΩ a 10 kΩ entre su salida y 5V.
- **Lado de potencia del SSR:** es independiente del Arduino. Al ser un SSR de corriente continua, respetar la polaridad (+/-) de la carga.
- **Interrupciones:** en la R4 Minima solo los pines 2 y 3 admiten interrupción externa. El inductivo en el pin 4 se lee por sondeo.
- **Monitor serie:** 115200 baudios.