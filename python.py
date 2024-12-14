import paho.mqtt.client as mqtt
import mysql.connector

# Configurações do banco de dados MySQL
db_config = {
    "host": "localhost",         # Endereço do servidor MySQL
    "user": "root",              # Usuário do MySQL
    "password": "34472063",      # Senha do MySQL
    "database": "sensores"       # Nome do banco de dados
}

# Função para conectar ao banco de dados
def connect_db():
    try:
        db = mysql.connector.connect(**db_config)
        print("Conexão com o banco de dados bem-sucedida.")
        return db
    except mysql.connector.Error as e:
        print(f"Erro ao conectar ao banco de dados: {e}")
        return None

# Função chamada ao conectar ao broker MQTT
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado ao broker MQTT com sucesso.")
        # Subscreve aos tópicos dos sensores
        client.subscribe("topico_sensor_temperatura")
        client.subscribe("topico_sensor_umidade")
        client.subscribe("topico_sensor_chuva")
        client.subscribe("topico_sensor_fogo")  # Adicionado o tópico do sensor de fogo
    else:
        print(f"Falha na conexão MQTT. Código de retorno: {rc}")

# Função chamada ao receber mensagens
def on_message(client, userdata, msg):
    print(f"Tópico: {msg.topic}, Mensagem: {msg.payload.decode()}")

    # Conectar ao banco de dados (caso desconecte durante o loop)
    db = userdata["db"]
    if not db.is_connected():
        print("Reconectando ao banco de dados...")
        db = connect_db()
        if not db:
            print("Falha ao reconectar ao banco de dados.")
            return
        userdata["db"] = db

    # Insere os dados no banco de dados
    try:
        cursor = db.cursor()
        query = "INSERT INTO leituras (topico, valor) VALUES (%s, %s)"
        cursor.execute(query, (msg.topic, msg.payload.decode()))
        db.commit()
        cursor.close()
        print("Dados inseridos no banco de dados com sucesso.")
    except Exception as e:
        print(f"Erro ao inserir no banco de dados: {e}")

# Configuração do cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conecta ao banco de dados
db = connect_db()
if not db:
    print("Não foi possível estabelecer conexão com o banco de dados. Encerrando script.")
    exit()

# Adiciona o banco ao contexto do cliente MQTT
client.user_data_set({"db": db})

# Conecta ao broker MQTT
try:
    client.connect("test.mosquitto.org", 1883, 60)
except Exception as e:
    print(f"Erro ao conectar ao broker MQTT: {e}")
    db.close()
    exit()

# Mantém o loop do cliente MQTT
try:
    client.loop_forever()
except KeyboardInterrupt:
    print("Encerrando script...")
    db.close()
