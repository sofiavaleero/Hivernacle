#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

// Configura tus credenciales de WiFi
const char *ssid = "iPhone de Paula";
const char *password = "f472547F";

// Direccion del servidor Sentilo
const char* host = "147.83.83.21";
const char* token = "f3ac6fb7b1657a691e28dec382a41e0182f2289bd13f120e399f008bb03ed2f9";
const char* provider = "grup_3-101@grup3_provider/";
const char* sensor1 = "humitat/";
const char* sensor2 = "temperatura/";
const char* sensor3 = "ldr1/";
const char* sensor4 = "ldr2/";
//const char* sensor = "light/";
// float LDRvalue = "500.0"
//const char* value = "500.0"; //Ajustar a quin serà el valor inicial de la lluminositat

//Actualització de comandes PUT POST GET DELETE
//GET
//const char* getreq = "GET /data/grup_3-101//humidity/20.9 HTTP/1.1\r\nIDENTITY_KEY: 9073208bedcc80ccdb7080bb24499bc92078e91a5997cb9e8c7b83d3643f9a53\r\n\r\n"
//const char* getreqLDR = "GET /data/grup_3-101//light/500.1 HTTP/1.1\r\nIDENTITY_KEY: 9073208bedcc80ccdb7080bb24499bc92078e91a5997cb9e8c7b83d3643f9a53\r\n\r\n"

// Configura los pines de los sensores
#define DHT_PIN 5      // Pin de datos del sensor DHT11
#define OLED_RESET 22   // Pin de datos de la pantalla
const int ldrPin1 = 32; // Pin del sensor LDR1
const int ldrPin2 = 34; // Pin del sensor LDR2
const float limitSuperiorTemperatura = 24;
const float limitSuperiorHumitat = 70;
const float limitSuperiorLluminositat = 700;
const float limitInferiorTemperatura = 18;
const float limitInferiorHumitat = 60;
const float limitInferiorLluminositat = 600;


DHT dht(DHT_PIN, DHT11);
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

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

  // Inicializa el display OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
}

// Función para calibrar la temperatura
float calibrateTemperature(float currentTemperature) {
  
  // Aplica la corrección
  float calibratedTemperature = currentTemperature - 3.0;

  return calibratedTemperature;
}

void putdata(const char* sensor, float value) {
  WiFiClient client;
  const int httpPort = 8081;

  if (client.connect(host, httpPort)) {
    client.print("PUT /data/");
    client.print(provider);
    client.print(sensor);
    client.print(value);
    client.print(" HTTP/1.1\r\nIDENTITY_KEY: ");
    client.print(token);
    client.print("\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Superado el tiempo de espera !");
        client.stop();
        return;
      }
    }

    client.stop();
  } else {
    Serial.println("Ha fallado la conexión");
  }
}

void analisidades(const char* sensor) {
  WiFiClient client;
  const int httpPort = 8081;

  if (client.connect(host, httpPort)) {
    client.print("GET /data/");
    client.print(provider);
    client.print(sensor);
    client.print(" HTTP/1.1\r\nIDENTITY_KEY: ");
    client.print(token);
    client.print("\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Superado el tiempo de espera !");
        client.stop();
        return;
      }
    }

    String response = client.readString();
    Serial.println("Response:");
    Serial.println(response);

    // Parse the response to extract values
    // Assuming the response format is like "value1,value2,value3,..."
    String delimiter = ",";
    int values[10];
    int index = 0;
    int pos = 0;

    while ((pos = response.indexOf(delimiter)) != -1 && index < 10) {
      String token = response.substring(0, pos);
      values[index] = token.toInt();
      response.remove(0, pos + delimiter.length());
      index++;
    }

    // Find maximum and minimum values
    int maxValue = values[0];
    int minValue = values[0];

    for (int i = 1; i < index; i++) {
      if (values[i] > maxValue) {
        maxValue = values[i];
      }

      if (values[i] < minValue) {
        minValue = values[i];
      }
    }

    Serial.print("Max value for ");
    Serial.print(sensor);
    Serial.print(": ");
    Serial.println(maxValue);

    Serial.print("Min value for ");
    Serial.print(sensor);
    Serial.print(": ");
    Serial.println(minValue);

    client.stop();
  } else {
    Serial.println("Ha fallado la conexión");
  }
}

void loop() {

  float humidity = dht.readHumidity();
  float temperature1 = dht.readTemperature();
  float temperature = calibrateTemperature(temperature1);
  // Lee el valor del sensor LDR
  int ldrValue1 = analogRead(ldrPin1);
  int ldrValue2 = analogRead(ldrPin2);

  putdata(sensor1, humidity);
  putdata(sensor2, temperature);
  putdata(sensor3, ldrValue1);
  putdata(sensor4, ldrValue2);

  analisidades(sensor1);
  analisidades(sensor2);
  analisidades(sensor3);
  analisidades(sensor4);

  // Limpia el display
  display.clearDisplay();

  if (temperature > limitSuperiorTemperatura || temperature < limitInferiorLluminositat) {
    display.setCursor(0, 40);
    display.print("¡Alerta! Alta temperatura");
  }

  if (humidity > limitSuperiorHumitat || humidity < limitInferiorHumitat) {
    display.setCursor(0, 50);
    display.print("¡Alerta! Alta temperatura");
  }

  if (ldrValue1 > limitSuperiorLluminositat || ldrValue2 > limitSuperiorLluminositat || ldrValue1 < limitInferiorLluminositat || ldrValue2 < limitInferiorLluminositat ) {
    display.setCursor(0, 60);
    display.print("¡Alerta! Baja luminosidad");
  }

  else {
  display.setCursor(0, 40);
  display.print("Todo en orden");
  }

  display.clearDisplay();

  // Imprime los valores en el display
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temperature);
  display.print(" C");

  //Valor Humedad
  display.setCursor(0, 10);
  display.print("Humedad: ");
  display.print(humidity);
  display.print(" %");

  //Valor Luz
  display.setCursor(0, 20);
  display.print("LDR1: ");
  display.print(ldrValue1);

  display.setCursor(0, 30);
  display.print("LDR2: ");
  display.print(ldrValue2);

  // Muestra los datos en el display
  display.display();

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
  delay(120000);
}

