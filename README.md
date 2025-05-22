# 💧 Monitoramento de Consumo de Água com ESP32 e MQTT

## 📌 i) Visão Geral

Este projeto utiliza um **ESP32** conectado a um **sensor de fluxo de água YF-S201** para monitorar em tempo real o consumo de água. Os dados são enviados via **protocolo MQTT** para o servidor **ThingsBoard**, onde são visualizados em dashboards interativos. O sistema também calcula automaticamente a tarifa de consumo com base na tabela da **Sabesp (2025)**. Um **LED indicador** atua como um atuador, acendendo quando há fluxo de água detectado.

---

## 💻 ii) Software Desenvolvido

O código-fonte está disponível em `main.ino`, escrito em C++ para Arduino. A lógica inclui:

- Leitura de pulsos do sensor YF-S201 (1 L ≈ 450 pulsos)
- Cálculo do fluxo instantâneo (L/min), volume acumulado e tarifa
- Controle de LED via pino digital (indicador de passagem de água)
- Envio dos dados via MQTT (com autenticação por token ThingsBoard)
- Serial log com valores de telemetria

### 🧩 Estrutura do Código
- `setup()`: Inicialização do Wi-Fi, MQTT e interrupção do sensor
- `loop()`: Verifica MQTT, calcula fluxo e volume, envia telemetria
- `calculateWaterBill()`: Função que calcula a conta com faixas tarifárias
- `reconnectMQTT()`: Gerencia reconexão automática ao broker

---

## 🔧 iii) Hardware Utilizado

### 🧠 Plataforma de Desenvolvimento
- **ESP32 DevKit v1**

### 💧 Sensores e Atuadores
- **Sensor de fluxo YF-S201**  
  - Rosca: 1/2 polegada  
  - Vazão típica: 1–30 L/min  
  - Tensão: 5V  
- **LED indicador** conectado ao pino GPIO 21

### 🧰 Protótipo e Encaixe
- **Protoboard 830 pontos**
- Conectores macho-fêmea para o sensor
- Fonte 5V USB via PC ou carregador
- **Caixa de acrílico ou impressão 3D** (sugestão de dimensões: 10cm x 8cm x 5cm)

---

## 🌐 iv) Interfaces, Protocolos e Módulos de Comunicação

### 🧠 Comunicação de dados
- **Wi-Fi**: Protocolo TCP/IP via biblioteca `WiFi.h`
- **MQTT**: Publicação de dados com `PubSubClient`
  - Servidor: `mqtt.thingsboard.cloud`
  - Porta: `1883`
  - Tópico: `v1/devices/me/telemetry`
  - Autenticação: Token do dispositivo ThingsBoard

### 📦 Formato das Mensagens MQTT
As mensagens são enviadas em formato JSON, como por exemplo:

```json
{
  "fluxo_L_min": 1.45,
  "volume_total_L": 15.3,
  "conta_R$": 0.127
}
