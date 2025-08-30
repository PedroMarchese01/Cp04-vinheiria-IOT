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
  }
}