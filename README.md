# Projeto de Monitoramento de Temperatura, Umidade e Luminosidade com ESP32

#Integrantes
- Pedro Marchese, RM563339
- Gustavo Neres, RM561785
- Rafaell Santiago, RM563486
- Enzo Augusto, RM562249


## Descrição

Este projeto utiliza o microcontrolador **ESP32** para monitorar a **temperatura**, **umidade** e a **luminosidade** do ambiente em tempo real. Os dados são coletados através dos sensores **DHT22** (para temperatura e umidade) e **LDR** (para luminosidade), e enviados para a plataforma **ThingSpeak** para visualização e monitoramento online.

O projeto é ideal para quem deseja aprender a integrar sensores com o ESP32 e a usar plataformas de IoT como o ThingSpeak para visualização de dados.

## Componentes Necessários

- 1x **ESP32** (qualquer modelo com Wi-Fi)
- 1x **Sensor DHT22** (para medir temperatura e umidade)
- 1x **LDR** (Light Dependent Resistor, para medir luminosidade)
- 1x **Resistor de 10kΩ** (para o LDR)
- Fios jumper para conexões

## Conexões

- **DHT22**:
  - **VCC**: 3.3V (pino do ESP32)
  - **GND**: GND (pino do ESP32)
  - **Data**: Pino **GPIO4** (pino configurado no código)

- **LDR**:
  - **Um lado do LDR**: Conectado a **3.3V** (pino do ESP32)
  - **Outro lado do LDR**: Conectado a **GPIO34** (pino configurado no código)
  - **Resistor de 10kΩ**: Entre o pino LDR e o GND (formando um divisor de tensão)

## Funcionalidade

1. O **ESP32** se conecta à rede **Wi-Fi** usando as credenciais fornecidas no código.
2. O código lê os valores de temperatura e umidade do **sensor DHT22** e o valor de luminosidade do **sensor LDR**.
3. Os dados são enviados a cada 5 segundos para o servidor **ThingSpeak** utilizando a API do ThingSpeak.
4. O **ThingSpeak** exibe os dados em tempo real no seu painel, permitindo monitoramento remoto.
5. No **monitor serial**, você pode ver as leituras dos sensores e o status da conexão com o Wi-Fi.

## Código

```cpp
#include <WiFi.h>             // Biblioteca para conectar o ESP32 a redes Wi-Fi
#include <DHT.h>              // Biblioteca para controlar sensores DHT (temperatura e umidade)
#include <HTTPClient.h>       // Biblioteca para enviar requisições HTTP (para ThingSpeak)

// ===== Configurações Wi-Fi =====
const char* ssid = ""; // Nome da rede Wi-Fi que o ESP32 vai conectar
const char* password = "";        // Senha da rede Wi-Fi

// ===== ThingSpeak =====
const char* thingspeak_api_key = "S3CC3E446CKRFI9G"; // API Key do canal ThingSpeak
const char* thingspeak_server = "api.thingspeak.com"; // Endereço do servidor ThingSpeak

// ===== DHT =====
#define DHTPIN 4                 // Pino do ESP32 conectado ao sensor DHT22
#define DHTTYPE DHT22            // Define o tipo do sensor (DHT22)
DHT dht(DHTPIN, DHTTYPE);       // Cria objeto do sensor DHT

// ===== LDR =====
const int ldrPin = 34;           // Pino conectado ao LDR (sensor de luminosidade)

void setup() {
  pinMode(ldrPin, INPUT);        // Configura o pino do LDR como entrada
  Serial.begin(115200);          // Inicializa comunicação Serial a 115200 bps
  dht.begin();                   // Inicializa o sensor DHT22
  setup_wifi();                  // Chama função para conectar o ESP32 à rede Wi-Fi
}

void setup_wifi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
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

void sendToThingSpeak(float temperatura, float umidade, int ldrValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String("http://") + thingspeak_server + "/update?api_key=" + thingspeak_api_key;
    url += "&field1=" + String(temperatura, 2);
    url += "&field2=" + String(umidade, 2);
    url += "&field3=" + String(ldrValue);

    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.print("ThingSpeak atualizado, código: ");
      Serial.println(httpCode);
    } else {
      Serial.print("Erro ao enviar para ThingSpeak: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

void loop() {
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 5000) {
    lastPublish = millis();

    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    int ldrValue = analogRead(ldrPin);

    if (!isnan(temperatura) && !isnan(umidade)) {
      sendToThingSpeak(temperatura, umidade, ldrValue);
    } else {
      Serial.println("Erro ao ler DHT!");
    }
  }
}
