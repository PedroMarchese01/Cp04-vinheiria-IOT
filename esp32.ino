// ===== LED =====
const int LED_PIN = 2;

// ===== Sensores =====
#define DHT_PIN 4
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

const int LDR_PIN = 34;

// ===== Funções sensores e LED =====
void sendSensorData() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  int ldrValue = analogRead(LDR_PIN);

  if (!isnan(temperatura) && !isnan(umidade)) {
    Serial.print("Temp: "); Serial.print(temperatura);
    Serial.print(" °C, Umid: "); Serial.print(umidade);
    Serial.print(" %, Lum: "); Serial.println(ldrValue);

    String payload = "t|" + String(temperatura, 2) +
                     "|h|" + String(umidade, 2) +
                     "|l|" + String(ldrValue);

    client.publish("/ul/esp32_01/attrs", payload.c_str());
  } else {
    Serial.println("Erro ao ler DHT!");
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();
  message.toUpperCase();

  Serial.print("Mensagem recebida em [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (message == "ON") {
    digitalWrite(LED_PIN, LOW);  // Liga LED
    Serial.println("LED LIGADO");
  } else if (message == "OFF") {
    digitalWrite(LED_PIN, HIGH); // Desliga LED
    Serial.println("LED DESLIGADO");
  } else if (message == "SEND") {
    Serial.println("Comando SEND recebido!");
    sendSensorData();
/ ===== Configurações Wi-Fi =====
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ===== Broker MQTT =====
const char* MQTT_BROKER = "172.184.155.68";
const int MQTT_PORT = 1883;

// ===== Cliente MQTT =====
WiFiClient espClient;
PubSubClient client(espClient);

// ===== Funções Wi-Fi e MQTT =====
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

void mqtt_reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao broker MQTT...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao broker!");
      client.subscribe("vinheria/atuadores"); 
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 2s");
      delay(2000);
    }
  }
}