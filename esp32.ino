/*
  Projeto: ESP32 + DHT11 + LDR + LED + MQTT
  Objetivo: Ler sensores, publicar dados via MQTT e receber comandos para LED
*/

// ==========================
// ===== Módulo Sensores =====
// ==========================

// ===== LED =====
const int LED_PIN = 2;  // LED interno do ESP32

// ===== Sensores =====
#define DHT_PIN 4
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);  // Inicializa DHT11

const int LDR_PIN = 34;  // Pino analógico do LDR

// ===== Função para enviar dados dos sensores =====
void sendSensorData() {
  float temperatura = dht.readTemperature(); // lê temperatura
  float umidade = dht.readHumidity();        // lê umidade
  int ldrValue = analogRead(LDR_PIN);        // lê luminosidade

  // Verifica se os valores são válidos
  if (!isnan(temperatura) && !isnan(umidade)) {
    Serial.print("Temp: "); Serial.print(temperatura);
    Serial.print(" °C, Umid: "); Serial.print(umidade);
    Serial.print(" %, Lum: "); Serial.println(ldrValue);

    // Monta payload no formato UltraLight
    String payload = "t|" + String(temperatura, 2) +
                     "|h|" + String(umidade, 2) +
                     "|l|" + String(ldrValue);

    // Publica no tópico MQTT
    client.publish("/ul/esp32_01/attrs", payload.c_str());
  } else {
    Serial.println("Erro ao ler DHT!");
  }
}

// ===== Função de callback MQTT =====
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();        // remove espaços
  message.toUpperCase(); // converte para maiúsculas

  Serial.print("Mensagem recebida em [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Comandos recebidos
  if (message == "ON") {
    digitalWrite(LED_PIN, LOW);  // Liga LED
    Serial.println("LED LIGADO");
  } else if (message == "OFF") {
    digitalWrite(LED_PIN, HIGH); // Desliga LED
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
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ===== Broker MQTT =====
const char* MQTT_BROKER = "172.184.155.68";
const int MQTT_PORT = 1883;

// ===== Cliente MQTT =====
WiFiClient espClient;
PubSubClient client(espClient);

// ===== Função de conexão Wi-Fi =====
void setup_wifi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar Wi-Fi");
  }
}

// ===== Função de reconexão MQTT =====
void mqtt_reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao broker!");
      client.subscribe("vinheria/atuadores"); // Tópico de comandos
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 2s");
      delay(2000);
    }
  }
}

// ==========================
// ===== Setup e Loop ======
// ==========================
void setup() {
  // Inicializa LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Desliga LED inicialmente

  // Inicializa sensores
  pinMode(LDR_PIN, INPUT);
  dht.begin();

  // Inicializa serial
  Serial.begin(115200);

  // Conecta Wi-Fi
  setup_wifi();

  // Configura MQTT
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(mqtt_callback);
}

void loop() {
  // Mantém conexão MQTT
  if (!client.connected()) {
    mqtt_reconnect();
  }
  client.loop();

  // Publica dados periodicamente (5 segundos)
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 5000) {
    lastPublish = millis();
    sendSensorData();
  }
}