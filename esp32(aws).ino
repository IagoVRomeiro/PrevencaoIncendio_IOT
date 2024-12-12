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
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

static const char aws_private_key[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAnEcsYyN85DZ+yN1M6tCBICJuSomVhnQ3HTzlRoATUKkyxFup
y2cfIj43fv6uwOcttx5LhUgzGsMdO6ITftLY7icDjY/FQXFVPKA9AG1K+gxLx1y7
D+X0sV4z60OzylXKaJIJFLJV9yjwvMT+4JvoJgl90cvvHMtX7YTlyN4+hwqqbsoX
ftNnHn81sR6vcMNTCWRTNsOJzqBkLMuwAiPJbe2I15rP4pVQjI/WtOdGjS9F36k/
H1iRMs+hNkx/c/CMd78kVVqIdkGsU+yYtJ9JqypB05niVOMT1s7WDyCqJ3uA3VDq
3OChD+fprH8Tg371QX+EsMw6my8Z/SFli8N79wIDAQABAoH/B++VUXN75BrgVVLn
lQUBiaA1+ZMDBNDh+5ocCnh2gqIU8eO1u56A9jwylUBZn3Id4C6ppxsDfVhrI5wX
YVlTqsRQad/jJG+c5Hc+ocbrB8eYEYej+Cq1QkyGVmwZtSymGqlPzL1BoEtz7iIS
z1UbdNMGkKDKg6vPyz/0az7d7f2IWZwObosIKg0cVW8nvyMlBGarWSoWdsurE9h/
2hlkJQ8i3BoGbjVTj0juWwPPb4AV/viGwj/IehlKQNvpujWKppDEgChJJyIF12+q
xb8qOBDIPVYkx866RfKUfEff4PSD8aLGRT3O2Dj1JXa+ikjnI2y5zHD5p9Z6lCg5
x0y5AoGBAMiJ15pc2cUQiob9YtGs8StlyXYCnwckB2ZmrqewFBHeeFjB1sCJDxP6
PeqE3gBc1DTb67ePDGkPLO2GPO3qjkdpotGuj/CcHBYNs0j2PlX6JDfRq3LyIkGs
HfsCwR8jQ37OZS3q/P0Y4yQPybSqiqXUwcRnrqRqFqUcZ0cK+umtAoGBAMd/r2ig
BDGp25Nt7VBbmThKFzDopcUPW08Kwubjz9okIOLpsqJonrdLNJDOo4wWi9/bPRa8
GhnXMSLoM61fmAy9O3i79i9IDm9kkcHJ7AgO8lLEhG0U8eREx3APHWB9uhe9LFxU
CXriueW7yzdiOlrIwG0ecM9cKYBeDL1PDXizAoGBAJpFuJBu17ULeZZJfekR42Jg
ZrCbaZ3DsNGtMrWb9oScofaeAEiT5/6VZ2ZwGp0qzLFxcg//QyvbWdPMjjj1VclI
UdJ/OQp//fdVSNRbYy8fRReP/BZsgcAF2faNcuYyaa02uI2vljpbF/pZDvImtIzI
dNxUavzfBFa22BjGMKrtAoGAH0iplAmLvtH5CcxyK2jBxS7xJg1FeXpoAiQY3iYZ
QQbrVUn6ZjdrtCiJ7K9KvDwI4Si7EqaanTS+ks/A0s4rxMuj2NH4z/9HrDYkGTR1
2+mkHFCOx6BCMl6Ku8rS+3+bv/89RpkzB18ocZsaSewQZI4YhsQAQrQcGfIlo0YT
QbsCgYEAiEyCUAzoi6V78MabCwyZsnhW0bTLcONSBW2/0SaRDRrnK0m9LFWL9T4W
kWqNAY9UuaooHfQdOn2gxspCvKF3VPzsLxazStYLqooQk2VN6b5oNjdDWmdhOEK+
gszRqQuXp9yWyzK6NrrdrnomUGfj8TxgzQhT8S4Z5BLYxPfERS8=
-----END RSA PRIVATE KEY-----

)KEY";

static const char aws_cert_ctr[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAJAGBDTLvApLwj0EIKLzhef1AKIDMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDEyMDQyMjA3
MzZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCcRyxjI3zkNn7I3Uzq
0IEgIm5KiZWGdDcdPOVGgBNQqTLEW6nLZx8iPjd+/q7A5y23HkuFSDMawx07ohN+
0tjuJwONj8VBcVU8oD0AbUr6DEvHXLsP5fSxXjPrQ7PKVcpokgkUslX3KPC8xP7g
m+gmCX3Ry+8cy1fthOXI3j6HCqpuyhd+02cefzWxHq9ww1MJZFM2w4nOoGQsy7AC
I8lt7YjXms/ilVCMj9a050aNL0XfqT8fWJEyz6E2TH9z8Ix3vyRVWoh2QaxT7Ji0
n0mrKkHTmeJU4xPWztYPIKone4DdUOrc4KEP5+msfxODfvVBf4SwzDqbLxn9IWWL
w3v3AgMBAAGjYDBeMB8GA1UdIwQYMBaAFAgIC/jhNbqk75gF1IgDx6w9gOm5MB0G
A1UdDgQWBBS4BKfHv4kRJ/UmfHQ4FkS8Q1V1CzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAdkdFSZhajfO+nubFeQ8mlDmn
jbnVNmyCS8d+/IcQh/2Eng7YxuaCggwTkgHV5EUlj2xtgIdIBBqjwbZjJ+wbiLW5
KwiNF3i4aXQRK61vlEVSfbmZKOVSo5W2SbOdIFANTULWiZpXcn4ssgj1DvlJKAAJ
4lfwIWt93/GXfYgtOiIeaCyhNLKEvyh5VI9mY3YaXlAxpnppodPl/O+AFOwuZukq
svMpBbyDhXrfS03pphGbVAcjuwtAn2BsPwcH46BET/s8PEzM2A1xDmJiADHctq38
ph9A06EDE7trTXDQKC373hruJJ2UdtVuE1Kh8h5zdjbcWT+DZogQNcDXMyUsng==
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
