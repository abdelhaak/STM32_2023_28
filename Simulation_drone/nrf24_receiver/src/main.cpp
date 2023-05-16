#include <Arduino.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(PB0, PC1); // Pines CE y CSN del módulo NRF24L01+

void setup() {
  Serial.begin(9600);
  radio.begin(); // Inicializar la comunicación con el módulo NRF24L01+
  radio.setChannel(115); // Establecer el canal de comunicación
  radio.setDataRate(RF24_250KBPS); // Establecer la velocidad de transmisión de datos
  radio.setPALevel(RF24_PA_MAX); // Establecer el nivel de potencia de la antena
  radio.openReadingPipe(0, 0x1234567890LL); // Establecer la dirección de origen
  radio.startListening(); // Habilitar la escucha
}

void loop() {
  if (radio.available()) {
    char payload[32];
    radio.read(&payload, sizeof(payload)); // Leer paquete recibido

    Serial.println(payload); // Escribir datos en el puerto serial
  }
}
