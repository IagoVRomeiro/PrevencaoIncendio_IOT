import paho.mqtt.client as mqtt
import mysql.connector

# Configurações do banco de dados MySQL
db = mysql.connector.connect(
    host="localhost",          # Endereço do servidor MySQL
    user="root",               # Usuário do MySQL
    password="34472063",      # Senha do MySQL
    database="sensores"        # Nome do banco de dados
)
cursor = db.cursor()

# Função chamada ao conectar ao broker MQTT
def on_connect(client, userdata, flags, rc):
    print("Conectado ao broker MQTT")
    # Subscreve aos tópicos dos sensores
    client.subscribe("topico_sensor_temperatura")
    client.subscribe("topico_sensor_umidade")
    client.subscribe("topico_sensor_chuva")

# Função chamada ao receber mensagens
def on_message(client, userdata, msg):
    print(f"Tópico: {msg.topic}, Mensagem: {msg.payload.decode()}")
    
    # Insere os dados no banco de dados
    try:
        query = "INSERT INTO leituras (topico, valor) VALUES (%s, %s)"
        cursor.execute(query, (msg.topic, msg.payload.decode()))
        db.commit()
        print("Dados inseridos no banco de dados.")
    except Exception as e:
        print(f"Erro ao inserir no banco: {e}")

# Configuração do cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conecta ao broker MQTT
client.connect("test.mosquitto.org", 1883, 60)

# Mantém o loop do cliente MQTT
client.loop_forever()
