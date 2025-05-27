#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID       "SEU_ENDERECO_WIFI"
#define WIFI_PASSWORD   "SUA_SENHA_WIFI"
#define TOKEN           "SEU_TOKEN_MQTT"
#define THINGSBOARD_SERVER "PATH_SEU_SERVIDOR_MQTT"

#define SENSOR_PIN 23
#define LED_PIN 21

// Tarifas Sabesp
const float TARIFA_FAIXA1 = 0.00669f;
const float TARIFA_FAIXA2 = 0.00933f;
const float TARIFA_FAIXA3 = 0.01433f;
const float TARIFA_FAIXA4 = 0.01713f;

WiFiClient espClient;
PubSubClient client(espClient);
volatile long pulseCount = 0;
volatile unsigned long firstPulseTime = 0;

unsigned long lastSendTime = 0;
float flowRate = 0.0;
float totalVolume = 0.0;
float waterBill = 0.0;

// Para medir tempo entre comando e ação no LED
unsigned long commandReceivedTime = 0;
unsigned long totalCommandLatency = 0;
int commandCount = 0;

// Para medir tempo entre detecção do sensor e envio
unsigned long totalSensorToMQTTTime = 0;
int sensorMessageCount = 0;

float calculateWaterBill(float volumeLitros) {
    float conta = 0.0f;
    if (volumeLitros <= 10000.0f) {
        conta = volumeLitros * TARIFA_FAIXA1;
    } else if (volumeLitros <= 20000.0f) {
        conta = 10000.0f * TARIFA_FAIXA1;
        conta += (volumeLitros - 10000.0f) * TARIFA_FAIXA2;
    } else if (volumeLitros <= 50000.0f) {
        conta = 10000.0f * TARIFA_FAIXA1;
        conta += 10000.0f * TARIFA_FAIXA2;
        conta += (volumeLitros - 20000.0f) * TARIFA_FAIXA3;
    } else {
        conta = 10000.0f * TARIFA_FAIXA1;
        conta += 10000.0f * TARIFA_FAIXA2;
        conta += 30000.0f * TARIFA_FAIXA3;
        conta += (volumeLitros - 50000.0f) * TARIFA_FAIXA4;
    }
    return conta;
}

void IRAM_ATTR countPulse() {
    pulseCount++;
    if (firstPulseTime == 0) {
        firstPulseTime = millis(); // marca o tempo do primeiro pulso
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    // Quando um comando for recebido
    payload[length] = '\0';
    String message = String((char*)payload);

    if (message == "ligar_led") {
        commandReceivedTime = millis();
        digitalWrite(LED_PIN, HIGH);

        unsigned long actionTime = millis();
        unsigned long latency = actionTime - commandReceivedTime;
        totalCommandLatency += latency;
        commandCount++;

        Serial.print("Comando MQTT -> LED em ");
        Serial.print(latency);
        Serial.println(" ms");
    } else if (message == "desligar_led") {
        digitalWrite(LED_PIN, LOW);
    }
}

void setup() {
    Serial.begin(19200);
    pinMode(SENSOR_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), countPulse, RISING);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("WiFi conectado!");

    client.setServer(THINGSBOARD_SERVER, 1883);
    client.setCallback(callback);
    reconnectMQTT();
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();

    unsigned long now = millis();
    if (now - lastSendTime > 2000) {
        detachInterrupt(digitalPinToInterrupt(SENSOR_PIN));
        long currentCount = pulseCount;
        unsigned long pulseStart = firstPulseTime;
        pulseCount = 0;
        firstPulseTime = 0;
        attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), countPulse, RISING);

        flowRate = (currentCount / 4.5f);
        float partialVolume = flowRate * (2.0f / 60.0f);
        totalVolume += partialVolume;
        waterBill = calculateWaterBill(totalVolume);

        if (currentCount > 0) {
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }

        // Tempo entre detecção e envio
        if (pulseStart > 0) {
            unsigned long sensorToMQTT = millis() - pulseStart;
            totalSensorToMQTTTime += sensorToMQTT;
            sensorMessageCount++;

            Serial.print("Sensor -> MQTT em ");
            Serial.print(sensorToMQTT);
            Serial.println(" ms");
        }

        StaticJsonDocument<256> doc;
        doc["fluxo_L_min"] = flowRate;
        doc["volume_total_L"] = totalVolume;
        doc["conta_R$"] = waterBill;

        if (commandCount > 0)
            doc["media_comando_led_ms"] = (float)totalCommandLatency / commandCount;
        if (sensorMessageCount > 0)
            doc["media_sensor_mqtt_ms"] = (float)totalSensorToMQTTTime / sensorMessageCount;

        char payload[256];
        serializeJson(doc, payload);
        client.publish("v1/devices/me/telemetry", payload);

        Serial.print("Fluxo: ");
        Serial.print(flowRate, 3);
        Serial.print(" L/min | Volume: ");
        Serial.print(totalVolume, 3);
        Serial.print(" L | Conta: R$ ");
        Serial.println(waterBill, 2);

        lastSendTime = now;
    }
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.println("Conectando ao ThingsBoard...");
        if (client.connect("ESP32Client", TOKEN, NULL)) {
            Serial.println("Conectado ao ThingsBoard!");
            client.subscribe("v1/devices/me/rpc/request/+"); // Para comandos do dashboard
        } else {
            Serial.print("Falha, rc=");
            Serial.print(client.state());
            delay(2000);
        }
    }
}