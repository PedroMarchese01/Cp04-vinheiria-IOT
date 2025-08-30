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