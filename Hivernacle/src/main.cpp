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
//const char* sensor = "light/";
// float LDRvalue = "500.0"
//const char* value = "500.0"; //Ajustar a quin serà el valor inicial de la lluminositat

//Actualització de comandes PUT POST GET DELETE
//PUT
const char* putreq = "PUT /data/grup_3-101@grup3_provider/humitat/20.9 HTTP/1.1\r\nIDENTITY_KEY: f3ac6fb7b1657a691e28dec382a41e0182f2289bd13f120e399f008bb03ed2f9\r\n\r\n";
//const char* putreqLDR = "PUT /data/grup_3-101//light/500.1 HTTP/1.1\r\nIDENTITY_KEY: 9073208bedcc80ccdb7080bb24499bc92078e91a5997cb9e8c7b83d3643f9a53\r\n\r\n";
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

void loop() {
  // Mostarmos que queremos conectar con el servidor Sentilo
  Serial.print("Abriendo la conexión con ");
  Serial.println(host);

  // Establecemos la conexion con el servidor Sentilo
  WiFiClient client;
  // Puerto HTTP
  const int httpPort = 8081;
  // Iniciamos la conexion
  if (client.connect(host, httpPort)) {
    // Lee la temperatura y la humedad
  float temperature1 = dht.readTemperature();
    //Calibra la temperatura
  float temperature = calibrateTemperature(temperature1);
  client.print("PUT /data/");
  client.print(provider);
  client.print(sensor2);
  client.print(temperature);
  client.print(" HTTP/1.1\r\n");
  client.print("IDENTITY_KEY: ");
  client.print(token);
  client.print("\r\n\r\n");

  float humidity = dht.readHumidity();
   // Enviamos el comando PUT definido
  client.print("PUT /data/");
  client.print(provider);
  client.print(sensor1);
  client.print(humidity);
  client.print(" HTTP/1.1\r\n");
  client.print("IDENTITY_KEY: ");
  client.print(token);
  client.print("\r\n\r\n");


  // Lee el valor del sensor LDR
  int ldrValue1 = analogRead(ldrPin1);
  int ldrValue2 = analogRead(ldrPin2);

  // Esperamos a que responda el servidor
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Superado el tiempo de espera !");
      client.stop();
      return;
    }
  }
  /*while(client.available()){
    String line = client.readStringUntil('/r');
    Serial.print(line);
  }*/

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
  delay(5000);

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
  
  if (!client.connect(host, httpPort)){
    // si ha respondido esperamos un poco para cerrar la conexion con el servidor
    Serial.print("Cerrando la conexión con ");
    Serial.println(host);
    timeout = millis();
    while (millis() - timeout < 200);
    // Cerramos la conexion
    client.stop();
  }
    // Espera 5 segundos antes de realizar la próxima lectura
  delay(15000);
    
  }
  else{
    // ¿hay algún error al conectar?
    Serial.println("Ha fallado la conexión");
    return;}
  
  
}

