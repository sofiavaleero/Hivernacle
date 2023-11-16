#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>

// Configura tus credenciales de WiFi
const char *ssid = "iPhone de: Gisela";
const char *password = "holita02";

// Configura los pines de los sensores
#define DHT_PIN 5      // Pin de datos del sensor DHT11
const int ldrPin1 = 32; // Pin del sensor LDR1
const int ldrPin2 = 34; // Pin del sensor LDR2

DHT dht(DHT_PIN, DHT11);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conéctate a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a la red WiFi");

  // Inicializa el sensor DHT11
  dht.begin();
  delay(5000);
}

void loop() {
  // Lee la temperatura y la humedad
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Lee el valor del sensor LDR
  int ldrValue1 = analogRead(ldrPin1);
  int ldrValue2 = analogRead(ldrPin2);

  // Imprime los valores
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Valor del sensor LDR1: ");
  Serial.println(ldrValue1);

  Serial.print("Valor del sensor LDR2: ");
  Serial.println(ldrValue2);

  // Espera 5 segundos antes de realizar la próxima lectura
  delay(5000);
}