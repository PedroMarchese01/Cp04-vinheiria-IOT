# Projeto ESP32 + DHT11 + LDR + LED + MQTT

## Autores
- Pedro Marchese - RM: 563339  
- Rafaell Santiago - RM: 563486  
- Gustavo Neres dos Santos - RM: 561785  
- Enzo Augusto Lima da Silva - RM: 562249  

## Descrição
Este projeto utiliza um **ESP32** para ler sensores de temperatura e umidade (DHT11) e luminosidade (LDR), enviar os dados via **MQTT** para um broker, e controlar um LED a partir de comandos recebidos via MQTT.

O objetivo é demonstrar integração de sensores, atuadores e comunicação IoT usando o protocolo **MQTT**.

---

## Componentes
- ESP32  
- Sensor DHT11  
- Sensor LDR  
- LED interno do ESP32  
- Resistor de 10kΩ para o LDR (se necessário)  
- Ambiente de desenvolvimento Arduino IDE  
- Broker MQTT (local ou remoto)  

---

## Funcionalidades
1. **Leitura de sensores**
   - DHT11: temperatura e umidade
   - LDR: luminosidade

2. **Publicação via MQTT**
   - Publica dados no formato UltraLight a cada 5 segundos  
   - Tópico: `/ul/esp32_01/attrs`

3. **Recebimento de comandos via MQTT**
   - Comando `ON` → Liga LED
   - Comando `OFF` → Desliga LED
   - Comando `SEND` → Envia imediatamente os dados dos sensores

---

## Configuração do Wi-Fi e MQTT
No código-fonte, configure os seguintes parâmetros:

```cpp
// Wi-Fi
const char* WIFI_SSID = "nome_da_rede";
const char* WIFI_PASSWORD = "senha_da_rede";

// Broker MQTT
const char* MQTT_BROKER = "IP_DO_BROKER";
const int MQTT_PORT = 1883;