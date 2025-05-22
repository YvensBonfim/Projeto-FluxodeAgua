# 💧 Monitor de Consumo de Água com ESP32 + MQTT + ThingsBoard

## 📄 Descrição Geral

Este projeto tem como objetivo o monitoramento do fluxo e volume de água utilizando um sensor de fluxo YF-S201 acoplado a um microcontrolador ESP32. Os dados de consumo são enviados em tempo real para a plataforma ThingsBoard via protocolo MQTT. Além disso, o sistema calcula o valor da conta de água com base nas tarifas vigentes da Sabesp para 2025 e aciona um LED quando há passagem de água detectada.

---

## 💻 Software Desenvolvido

O código foi desenvolvido na linguagem C++ utilizando a Arduino IDE. Ele realiza:

- Conexão com a rede Wi-Fi
- Leitura de pulsos do sensor YF-S201 (cada 450 pulsos ≈ 1 litro)
- Cálculo do fluxo em L/min e volume acumulado
- Cálculo do custo conforme as faixas tarifárias
- Acionamento do LED ao detectar fluxo
- Envio de dados em JSON via MQTT para o ThingsBoard

### Principais Bibliotecas

- `WiFi.h`
- `PubSubClient.h`
- `ArduinoJson.h`

---

## 🧰 Hardware Utilizado

| Componente        | Descrição                                                                 |
|-------------------|---------------------------------------------------------------------------|
| ESP32 DevKit v1   | Microcontrolador com suporte Wi-Fi e múltiplas GPIOs                      |
| Sensor YF-S201    | Sensor de fluxo de água com saída digital (pulsos)                        |
| LED + Resistor    | Indicador visual para detecção de fluxo (GPIO 21)                         |
| Protoboard        | Montagem rápida dos componentes                                           |
| Jumpers           | Conexões elétricas                                                        |
| Impressão 3D      | (opcional) Suporte e caixa para proteção do sensor e ESP32                |

**Medidas (caixa opcional):**  
- Dimensões sugeridas: 10cm x 6cm x 5cm  
- Abertura para USB, LED visível e furos para cabos do sensor

---

## 🌐 Interfaces, Protocolos e Comunicação

- **Wi-Fi (TCP/IP):** utilizado para conectar o ESP32 à internet.
- **MQTT (porta 1883):** protocolo leve de publicação/assinatura. As mensagens são publicadas em `v1/devices/me/telemetry` usando o token do dispositivo.
- **JSON:** formato utilizado para envio dos dados de telemetria.
- **Interrupts:** usados para contagem de pulsos do sensor (alta precisão).
- **Digital Output:** aciona o LED com base na detecção de fluxo.

---

## ☁️ Integração com ThingsBoard (MQTT Broker)

A integração entre o ESP32 e o servidor MQTT do ThingsBoard é realizada da seguinte forma:

1. O ESP32 conecta-se à rede Wi-Fi local.
2. Estabelece conexão com o broker `mqtt.thingsboard.cloud`, utilizando como autenticação um token único gerado na plataforma.
3. A cada 2 segundos, o ESP32 calcula o fluxo e volume de água, atualiza o valor da conta e envia esses dados via JSON para o tópico MQTT `v1/devices/me/telemetry`.
4. O ThingsBoard recebe, armazena e permite visualização em tempo real em dashboards personalizados.

---

## 🛠️ Passo a Passo para Reproduzir o Projeto

### 1️⃣ Materiais Necessários

- 1x ESP32 DevKit v1  
- 1x Sensor de fluxo YF-S201  
- 1x LED comum  
- 1x Resistor 220Ω  
- Protoboard + Jumpers  
- Cabo micro-USB  
- Conta gratuita no [ThingsBoard Cloud](https://thingsboard.cloud/signup)

---

### 2️⃣ Montagem do Hardware

**Conexões do sensor YF-S201:**

| Fio do Sensor | Conectar ao ESP32 |
|---------------|-------------------|
| Vermelho      | VIN (5V)          |
| Preto         | GND               |
| Amarelo       | GPIO 23           |

**Conexão do LED com resistor:**

- Anodo (+) do LED → resistor 220Ω → GPIO 21  
- Catodo (–) do LED → GND

---

### 3️⃣ Upload do Código no Arduino IDE

1. Instale a [Arduino IDE](https://www.arduino.cc/en/software)  
2. Adicione a URL do ESP32 em **Arquivo > Preferências**:  
