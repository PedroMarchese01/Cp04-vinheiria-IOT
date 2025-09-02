/*
Nomes:
Pedro Marchese RM:563339
Rafaell Santiago RM:563486
Gustavo Neres dos Santos RM: 561785
Enzo Augusto Lima da Silva RM:562249


*/

#include <WiFi.h>             // Biblioteca para conectar o ESP32 a redes Wi-Fi
#include <DHT.h>              // Biblioteca para controlar sensores DHT (temperatura e umidade)
#include <HTTPClient.h>       // Biblioteca para enviar requisições HTTP (para ThingSpeak)

// ===== Configurações Wi-Fi =====
const char* ssid = "Wokwi-GUEST"; // Nome da rede Wi-Fi que o ESP32 vai conectar
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

// ===== Funções de configuração =====
void setup() {
  pinMode(ledPin, OUTPUT);       // Configura o pino do LED como saída
  digitalWrite(ledPin, HIGH);    // Inicializa o LED desligado (LED interno do ESP32 é LOW ativo)
  pinMode(ldrPin, INPUT);        // Configura o pino do LDR como entrada
  Serial.begin(115200);          // Inicializa comunicação Serial a 115200 bps
  dht.begin();                   // Inicializa o sensor DHT22
  setup_wifi();                  // Chama função para conectar o ESP32 à rede Wi-Fi
}

// ===== Conexão Wi-Fi =====
void setup_wifi() {
  Serial.println("Conectando ao Wi-Fi..."); // Mensagem informando que está tentando conectar
  WiFi.begin(ssid, password);               // Inicia tentativa de conexão com SSID e senha
  int tries = 0;                            // Contador de tentativas de conexão
  while (WiFi.status() != WL_CONNECTED && tries < 20) { // Enquanto não conectar e tentativas < 20
    delay(500);                             // Aguarda 500ms
    Serial.print(".");                      // Mostra progresso no Serial
    tries++;                                // Incrementa contador de tentativas
  }
  if (WiFi.status() == WL_CONNECTED) {      // Verifica se conseguiu conectar
    Serial.println("\nWi-Fi conectado!");   // Mensagem de sucesso
    Serial.print("IP: ");                    // Mostra o endereço IP obtido
    Serial.println(WiFi.localIP());         // Imprime IP do ESP32 na rede
  } else {                                  // Se não conseguir conectar
    Serial.println("\nFalha ao conectar Wi-Fi"); // Mensagem de erro
  }
}

// ===== Função para imprimir dados no Serial =====
void printSensorData(float temperatura, float umidade, int ldrValue) {
  Serial.println("===== Dados dos Sensores ====="); // Cabeçalho de leitura
  Serial.print("Temperatura: ");                     // Texto descritivo
  Serial.print(temperatura, 2);                     // Valor da temperatura com 2 casas decimais
  Serial.println(" °C");                             // Unidade de medida

  Serial.print("Umidade: ");                         // Texto descritivo
  Serial.print(umidade, 2);                          // Valor da umidade com 2 casas decimais
  Serial.println(" %");                               // Unidade de medida

  Serial.print("Luminosidade (LDR): ");             // Texto descritivo
  Serial.println(ldrValue);                          // Valor lido do LDR (0-4095 no ESP32)

  Serial.println("=============================");  // Separador visual no Serial
}

// ===== Função para enviar dados ao ThingSpeak =====
void sendToThingSpeak(float temperatura, float umidade, int ldrValue) {
  if (WiFi.status() == WL_CONNECTED) {  // Verifica se está conectado ao Wi-Fi
    HTTPClient http;                    // Cria objeto HTTP para enviar requisição

    // Monta URL para atualizar o canal ThingSpeak com os valores dos sensores
    String url = String("http://") + thingspeak_server + "/update?api_key=" + thingspeak_api_key;
    url += "&field1=" + String(temperatura, 2); // Temperatura no campo 1
    url += "&field2=" + String(umidade, 2);    // Umidade no campo 2
    url += "&field3=" + String(ldrValue);      // LDR no campo 3

    http.begin(url);               // Inicializa requisição HTTP
    int httpCode = http.GET();     // Envia GET para ThingSpeak

    if (httpCode > 0) {            // Se recebeu resposta positiva
      Serial.print("ThingSpeak atualizado, código: "); 
      Serial.println(httpCode);    // Mostra código de resposta HTTP
    } else {                       // Se ocorreu erro na requisição
      Serial.print("Erro ao enviar para ThingSpeak: "); 
      Serial.println(httpCode);    // Mostra código de erro
    }

    http.end();                     // Finaliza conexão HTTP
  }
}

// ===== Loop principal =====
void loop() {
  static unsigned long lastPublish = 0;           // Variável para controlar intervalo de publicação
  if (millis() - lastPublish > 5000) {           // Se passaram mais de 5 segundos
    lastPublish = millis();                       // Atualiza tempo da última publicação

    float temperatura = dht.readTemperature();   // Lê temperatura do DHT22
    float umidade = dht.readHumidity();          // Lê umidade do DHT22
    int ldrValue = analogRead(ldrPin);          // Lê valor do LDR (0-4095)

    if (!isnan(temperatura) && !isnan(umidade)) { // Verifica se leituras são válidas
      printSensorData(temperatura, umidade, ldrValue); // Mostra dados no Serial
      sendToThingSpeak(temperatura, umidade, ldrValue); // Envia dados para ThingSpeak
    } else {                                     // Caso haja erro na leitura do DHT
      Serial.println("Erro ao ler DHT!");       // Mensagem de erro
    }
  }
}
//F!@P25.IOT
