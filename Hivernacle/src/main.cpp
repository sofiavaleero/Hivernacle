#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Configura tus credenciales de WiFi
const char *ssid = "iPhone de Paula";
const char *password = "f472547F";

// Configura los pines de los sensores
#define DHT_PIN 4      // Pin de datos del sensor DHT11
const int ldrPin = A0; // Pin del sensor LDR

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
}

void loop() {
  // Lee la temperatura y la humedad
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Lee el valor del sensor LDR
  int ldrValue = analogRead(ldrPin);

  // Imprime los valores
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Valor del sensor LDR: ");
  Serial.println(ldrValue);

  // Espera 10 segundos antes de realizar la próxima lectura
  delay(10000);
}
