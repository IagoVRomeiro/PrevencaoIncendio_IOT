#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>


// Defina as credenciais da sua rede WiFi
const char* ssid = "IAGOBLINK";
const char* password = "34472063";

// Defina os parâmetros do AWS IoT
const char* mqtt_server = "a1l3w3pl2iig2-ats.iot.sa-east-1.amazonaws.com";
const int mqtt_port = 8883;
const char* mqtt_topic = "esp32/topic"; // Substitua pelo seu tópico


static const char aws_cert_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";

static const char aws_private_key[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----

)KEY";

static const char aws_cert_ctr[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----

)KEY";


// Instância de cliente WiFi
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  // Inicia a comunicação serial
  Serial.begin(115200);
  delay(1000);

  // Conecta-se à rede WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Configura o cliente MQTT para usar o AWS IoT
espClient.setCACert(aws_cert_ca);
espClient.setCertificate(aws_cert_ctr);
espClient.setPrivateKey(aws_private_key);


  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);

  // Conecta ao broker MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void loop() {
  // Mantém a conexão com o broker MQTT
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      if (client.connect("ESP32Client")) {
        Serial.println("connected");
        client.subscribe(mqtt_topic);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        delay(5000);
      }
    }
  }
  client.loop();

  // Envia uma mensagem a cada 5 segundos
  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    String message = "Hello from ESP32 at " + String(now);
    client.publish(mqtt_topic, message.c_str());
    Serial.println("Message sent: " + message);
  }
}

// Função de callback para quando uma mensagem for recebida
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
