/*
Nomes:
Pedro Marchese RM:563339
Rafaell Santiago RM:563486
Gustavo Neres dos Santos RM: 561785
Enzo Augusto Lima da Silva RM:562249

  Projeto: ESP32 + DHT11 + LDR + LED + MQTT
  Objetivo: Ler sensores, publicar dados via MQTT e receber comandos para LED
*/

// ==========================
// ===== Módulo Sensores =====
// ==========================

// ===== LED =====
const int LED_PIN = 2;  // Define o pino do LED interno do ESP32

// ===== Sensores =====
#define DHT_PIN 4          // Pino digital para o sensor DHT11
#define DHT_TYPE DHT11     // Tipo do sensor DHT
DHT dht(DHT_PIN, DHT_TYPE); // Inicializa o sensor DHT

const int LDR_PIN = 34;   // Pino analógico do LDR

// ===== Função para enviar dados dos sensores =====
void sendSensorData() {
  float temperatura = dht.readTemperature(); // Lê a temperatura do DHT
  float umidade = dht.readHumidity();        // Lê a umidade do DHT
  int ldrValue = analogRead(LDR_PIN);        // Lê o valor do LDR (luminosidade)

  // Verifica se os valores do DHT são válidos
  if (!isnan(temperatura) && !isnan(umidade)) {
    // Exibe os valores no monitor serial
    Serial.print("Temp: "); Serial.print(temperatura);
    Serial.print(" °C, Umid: "); Serial.print(umidade);
    Serial.print(" %, Lum: "); Serial.println(ldrValue);

    // Monta a string de payload no formato UltraLight
    String payload = "t|" + String(temperatura, 2) +
                     "|h|" + String(umidade, 2) +
                     "|l|" + String(ldrValue);

    // Publica os dados no tópico MQTT
    client.publish("/ul/esp32_01/attrs", payload.c_str());
  } else {
    Serial.println("Erro ao ler DHT!"); // Mensagem de erro caso falhe a leitura
  }
}

// ===== Função de callback MQTT =====
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message = "";               // Inicializa string para armazenar mensagem

  // Constrói a mensagem a partir do payload recebido
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();        // Remove espaços extras
  message.toUpperCase(); // Converte mensagem para maiúsculas

  // Exibe tópico e mensagem recebida no monitor serial
  Serial.print("Mensagem recebida em [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Verifica comandos recebidos
  if (message == "ON") {
    digitalWrite(LED_PIN, LOW);  // Liga o LED
    Serial.println("LED LIGADO");
  } else if (message == "OFF") {
    digitalWrite(LED_PIN, HIGH); // Desliga o LED
    Serial.println("LED DESLIGADO");
  } else if (message == "SEND") {
    Serial.println("Comando SEND recebido!");
    sendSensorData();            // Envia dados dos sensores
  }
}

// ==========================
// ===== Módulo Conexão =====
// ==========================

// ===== Configurações Wi-Fi =====
const char* WIFI_SSID = "Wokwi-GUEST"; // Nome da rede Wi-Fi
const char* WIFI_PASSWORD = "";        // Senha da rede Wi-Fi

// ===== Broker MQTT =====
const char* MQTT_BROKER = "172.184.155.68"; // IP do broker MQTT
const int MQTT_PORT = 1883;                 // Porta do broker MQTT

// ===== Cliente MQTT =====
WiFiClient espClient;        // Cliente TCP/IP
PubSubClient client(espClient); // Cliente MQTT

// ===== Função de conexão Wi-Fi =====
void setup_wifi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Inicia conexão Wi-Fi
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) { // Tenta 20 vezes
    delay(500);
    Serial.print(".");
    tries++;
  }
  if (WiFi.status() == WL_CONNECTED) { // Se conectado
    Serial.println("\nWi-Fi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else { // Se falhar
    Serial.println("\nFalha ao conectar Wi-Fi");
  }
}

// ===== Função de reconexão MQTT =====
void mqtt_reconnect() {
  while (!client.connected()) { // Enquanto não conectado
    Serial.println("Conectando ao broker MQTT...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX); // Gera ID aleatório
    if (client.connect(clientId.c_str())) { // Tenta conectar
      Serial.println("Conectado ao broker!");
      client.subscribe("vinheria/atuadores"); // Inscreve no tópico de comandos
    } else {
      Serial.print("Falha, rc=");   // Exibe erro
      Serial.print(client.state());
      Serial.println(" tentando novamente em 2s");
      delay(2000);                   // Aguarda 2 segundos antes de tentar novamente
    }
  }
}

// ==========================
// ===== Setup e Loop ======
// ==========================
void setup() {
  pinMode(LED_PIN, OUTPUT);   // Configura pino do LED como saída
  digitalWrite(LED_PIN, HIGH);// Desliga LED inicialmente
  pinMode(LDR_PIN, INPUT);    // Configura LDR como entrada
  dht.begin();                // Inicializa sensor DHT
  Serial.begin(115200);       // Inicializa serial
  setup_wifi();               // Conecta ao Wi-Fi
  client.setServer(MQTT_BROKER, MQTT_PORT); // Configura broker MQTT
  client.setCallback(mqtt_callback);        // Define função de callback MQTT
}

void loop() {
  if (!client.connected()) { // Mantém conexão MQTT
    mqtt_reconnect();
  }
  client.loop();             // Processa mensagens MQTT recebidas

  // Publica dados a cada 5 segundos
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 5000) {
    lastPublish = millis();
    sendSensorData();        // Envia dados dos sensores
  }
}
