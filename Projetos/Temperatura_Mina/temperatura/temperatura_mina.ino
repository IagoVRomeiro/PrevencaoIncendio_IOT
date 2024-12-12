#include <DHTesp.h>

// Defina o GPIO onde o DHT11 está conectado
#define DHTPIN 4  // GPIO4

DHTesp dht;

void setup() {
  Serial.begin(115200);
  dht.setup(DHTPIN, DHTesp::DHT11); // Configura o pino e o tipo de sensor
}

void loop() {
  TempAndHumidity data = dht.getTempAndHumidity();

  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Falha ao ler do sensor DHT11!");
    return;
  }

  Serial.print("Temperatura: ");
  Serial.print(data.temperature);
  Serial.println(" *C");

  Serial.print("Umidade: ");
  Serial.print(data.humidity);
  Serial.println(" %");
  
  delay(2000);  // Aguarde 2 segundos entre leituras
}
