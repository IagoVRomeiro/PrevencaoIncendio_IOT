import paho.mqtt.client as mqtt
import mysql.connector
import json
from datetime import datetime

# Configuração do banco de dados MySQL
db_config = {
    'host': 'localhost',
    'user': 'root',
    'password': '34472063',
    'database': 'sensores'
}

# Configuração do MQTT
BROKER = "test.mosquitto.org"
PORT = 1883
TOPICS = [
    "topico_sensor_chuva", 
    "topico_sensor_fogo", 
    "topico_sensor_temperatura_estado", 
    "topico_sensor_temperatura_valor",
    "topico_sensor_umidade_estado",
    "topico_sensor_umidade_valor"
]

# Conectando ao banco de dados MySQL
def connect_db():
    return mysql.connector.connect(
        host=db_config['host'],
        user=db_config['user'],
        password=db_config['password'],
        database=db_config['database']
    )

# Função de callback quando uma mensagem é recebida
def on_message(client, userdata, msg):
    try:
        # Parseando a mensagem
        message = msg.payload.decode('utf-8')
        topic = msg.topic
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        # Preparando os dados para inserir na tabela
        data = {
            "fogo": "Fogo" if "Fogo" in message else "Sem Fogo",
            "chuva_estado": "Chuva" if "Chuva" in message else "Sem Chuva",
            "chuva_valor": 0,  # Defina o valor correto se disponível
            "temperatura_estado": message if "Frio" in message or "Agradável" in message or "Quente" in message else "Aguardando",
            "temperatura_valor": float(message.split()[1]) if "Temperatura" in message else 0.0,
            "data_hora": timestamp
        }

        # Conectando ao banco de dados e inserindo os dados
        db = connect_db()
        cursor = db.cursor()

        # Inserção no banco de dados
        sql = """
            INSERT INTO dados_sensores (fogo, data_hora, chuva_estado, chuva_valor, temperatura_estado, temperatura_valor)
            VALUES (%s, %s, %s, %s, %s, %s)
        """
        values = (
            data["fogo"],
            data["data_hora"],
            data["chuva_estado"],
            data["chuva_valor"],
            data["temperatura_estado"],
            data["temperatura_valor"]
        )

        cursor.execute(sql, values)
        db.commit()

        cursor.close()
        db.close()
        print(f"Dados inseridos: {data}")
    except Exception as e:
        print(f"Erro ao processar a mensagem: {e}")

# Conectando-se ao MQTT
def on_connect(client, userdata, flags, rc):
    print("Conectado ao broker MQTT com código de resultado " + str(rc))
    for topic in TOPICS:
        client.subscribe(topic)
    print(f"Inscrito nos tópicos: {TOPICS}")

# Configuração do cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conectando ao broker MQTT
client.connect(BROKER, PORT, 60)

# Loop MQTT para receber mensagens
client.loop_forever()
