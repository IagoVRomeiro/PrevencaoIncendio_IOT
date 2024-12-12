#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <TMP36.h>
#include "dht.h"

#define LED_BUILTIN 2
#define PIN_LED 2

/* Definições para o MQTT */
#define TOPICO_SUBSCRIBE_LED "topico_led"
#define TOPICO_PUBLISH_TEMPERATURA "topico_sensor_temperatura"
#define TOPICO_PUBLISH_UMIDADE "topico_sensor_umidade"
#define TOPICO_PUBLISH_CHUVA "topico_sensor_chuva"
#define ID_MQTT "IoT_PUC_SG_mqtt"  // ID MQTT


const char* SSID = "MotorolaEdge20_Arthur";      // Rede Wi-Fi
const char* PASSWORD = "12131415";  // Senha do Wi-Fi
const char* BROKER_MQTT = "test.mosquitto.org";
int BROKER_PORT = 1883;  // Porta do Broker MQTT


// Pinos dos sensores
#define PIN_SENSOR_CHUVA 15    //sensor de chuva
#define PIN_SENSOR_UMIDADE 33  //sensor umidade 36
#define pinoDHT11 18//sensor temperatura pino 18


//Sensor de fogo, buzzer e led
//Olhar se esse pinos vão funcionar
dht DHT; //VARIÁVEL DO TIPO DHT

const int fireSensorPin = 32;
const int buzzerPin = 13;
const int ledPin = 26;

#define rele 5 //rele para controlar o arduino

// Variáveis e objetos globais
WiFiClient espClient;
PubSubClient MQTT(espClient);

/* Prototypes */
void initWiFi(void);
void initMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void VerificaConexoesWiFIEMQTT(void);
void verificaChuva(void);
void leUmidade(void);
void leTemperatura(void);

/* Implementações */
void initWiFi(void) {
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  reconnectWiFi();
}

void initMQTT(void) {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("Chegou via MQTT: ");
  Serial.println(msg);

  if (msg.equals("L")) {
    digitalWrite(PIN_LED, HIGH);
    Serial.println("LED aceso");
  } else if (msg.equals("D")) {
    digitalWrite(PIN_LED, LOW);
    Serial.println("LED apagado");
  }
}

void reconnectMQTT(void) {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
      MQTT.subscribe(TOPICO_SUBSCRIBE_LED);
    } else {
      Serial.println("Falha na conexão. Tentando novamente em 2s");
      delay(2000);
    }
  }
}

void VerificaConexoesWiFIEMQTT(void) {
  if (!MQTT.connected()) {
    reconnectMQTT();
  }
  reconnectWiFi();
}

void reconnectWiFi(void) {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado ao Wi-Fi.");
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());
}

void verificaChuva(void) {
  int estadoChuva = digitalRead(PIN_SENSOR_CHUVA);
  if (estadoChuva == HIGH) {
    MQTT.publish(TOPICO_PUBLISH_CHUVA, "Sem Chuva");
    Serial.println("Publicando no MQTT: Sem Chuva");
  } else {
    MQTT.publish(TOPICO_PUBLISH_CHUVA, "Chuva");
    Serial.println("Publicando no MQTT: Chuva");
  }
}

/*
void leUmidade(void) {
  int umidade = analogRead(PIN_SENSOR_UMIDADE); 
  char umidade_str[10];
 // sprintf(umidade_str, "%d", umidade); 

  MQTT.publish(TOPICO_PUBLISH_UMIDADE, umidade_str);
  Serial.print("Publicando umidade: ");
  Serial.println(umidade_str);

  if(umidade > 2000){
    //Baixa umidade: ligar motor
    digitalWrite(rele, HIGH); // motor ativado
    Serial.println("Baixa umidade: Motor ligado");
  }else{
    //Alta umidade: desligar motor
    digitalWrite(rele, LOW); // motor desativado
    Serial.println("Alta umidade: Motor desligado");
  }
}*/

void leTemperatura(void) {
  DHT.read11(pinoDHT11); //LÊ AS INFORMAÇÕES DO SENSOR
  Serial.print("Temperatura: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(DHT.temperature); //IMPRIME NA SERIAL O VALOR DE UMIDADE MEDIDO E REMOVE A PARTE DECIMAL
  Serial.println("*C"); //IMPRIME O TEXTO NA SERIAL
  delay(2000); //INTERVALO DE 2 SEGUNDOS * NÃO DIMINUIR ESSE VALOR
}

void verificaFogo() {
  // Lê o valor do sensor de fogo
  int fireDetected = digitalRead(fireSensorPin);

  // Imprime o valor lido pelo sensor na porta serial
  Serial.println(fireDetected);

  if (fireDetected == LOW) {
    // Quando o fogo for detectado
    Serial.println("Ta pegando fogo bicho");
    // Aciona o buzzer com bipes como sirene
    for (int i = 0; i < 8; i++) {
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      delay(100);
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
  } else {
    // Quando não há fogo
    Serial.println("Sem fogo bicho");
    // Desliga o buzzer e o LED
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
  }
}

void verificaUmidade() {
  // Lê o valor do sensor de umidade (valores entre 0 e 1023)
  int umidade = analogRead(PIN_SENSOR_UMIDADE);

  // Mostra o valor da umidade na serial
  Serial.print("Valor da umidade: ");
  Serial.println(umidade);

  // Controla o motor com base na umidade
  if (umidade < 340) { // Ajuste o limite conforme necessário
    // Baixa umidade: ligar o motor
    digitalWrite(rele, LOW);  // Ativa o motor
    Serial.println("Baixa Umidade: Motor Ligado");
  } else {
    // Alta umidade: desligar o motor
    digitalWrite(rele, HIGH); // Desativa o motor
    Serial.println("Alta Umidade: Motor Desligado");
  }
  
  // Pequeno atraso para evitar leituras muito rápidas
  delay(1000);
}


void setup() {
  Serial.begin(9600); 
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SENSOR_CHUVA, INPUT); 

  reconnectWiFi(); // Conecta ao Wi-Fi
  initMQTT(); // Inicializa o MQTT
  reconnectMQTT(); // Conecta ao Broker MQTT

  // Define os pinos do sensor, buzzer e LED como entrada ou saída
  pinMode(fireSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  //Definindo rele para controle do motor
  pinMode(rele, OUTPUT);
  Serial.begin(9600);
  digitalWrite(rele, HIGH);

}

void loop() {
  VerificaConexoesWiFIEMQTT(); // Verifica conexões Wi-Fi e MQTT

  verificaChuva(); // Lê o estado do sensor de chuva
  //leUmidade(); // Lê a umidade
  leTemperatura(); // Lê a temperatura
  delay(2000);

  MQTT.loop(); // Mantém a conexão MQTT viva
  delay(2000); // Espera 2 segundos antes de repetir

  verificaFogo();  // Chama a função para verificar o sensor de fogo
  delay(500); 

  verificaUmidade(); // Chama a função para verificar o sensor de umidade
  



}

