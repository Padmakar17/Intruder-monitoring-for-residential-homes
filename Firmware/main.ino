#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ================= INPUT SENSORS ================= */
#define IR_PIN        13
#define REED_PIN      15
#define SHOCK_PIN     16

/* ================= OUTPUT DEVICES ================= */
#define GREEN_LED     33
#define RED_LED       32
#define BUZZER        25

/* ================= RFID ================= */
#define SS_PIN        5
#define RST_PIN       27

/* ================= OLED ================= */
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* ================= WIFI ================= */
const char* ssid     = "1718";
const char* password = "123456789";

/* ================= THINGSBOARD MQTT ================= */
const char* mqtt_server = "mqtt .thingsboard.cloud";
const int   mqtt_port   = 1883;
const char* accessToken = "gH4H0Pa39Afejd6a35kI";

/* ================= MQTT CLIENT ================= */
WiFiClient espClient;
PubSubClient client(espClient);

/* ================= RFID ================= */
MFRC522 rfid(SS_PIN, RST_PIN);

/* ================= AUTHORIZED UID ================= */
String authorizedUID1 = "F332FA2C";
String authorizedUID2 = "14C34002";

/* ================= SYSTEM VARIABLES ================= */
bool systemSleep = false;
int countdown = 0;
unsigned long lastTick = 0;
String activeUID = "";

/* ================= SHOCK FLAG ================= */
volatile bool shockDetected = false;

/* ================= ISR ================= */
void IRAM_ATTR shockISR() {
  shockDetected = true;
}

/* ================= OLED ================= */
void showOLED(String l1, String l2 = "", String l3 = "") {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(l1);
  if (l2 != "") display.println(l2);
  if (l3 != "") display.println(l3);
  display.display();
}

/* ================= MQTT PUBLISH ================= */
void mqttPublish(const char* msg) {
  if (client.connected()) {
    String payload = "{";
    payload += "\"event\":\"" + String(msg) + "\"";
    payload += "}";

    client.publish("v1/devices/me/telemetry", payload.c_str());
  }
  Serial.print("[MQTT] ");
  Serial.println(msg);
}

/* ================= ALARM ================= */
void triggerAlarm(String reason) {
  showOLED("INTRUSION ALERT", reason);
  mqttPublish(reason.c_str());

  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(3000);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

/* ================= WIFI CONNECT ================= */
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("[WIFI] Connecting");

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
    Serial.println("\n[WIFI] Connected");
  else
    Serial.println("\n[WIFI] Failed");
}

/* ================= MQTT CONNECT ================= */
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("[MQTT] Connecting to ThingsBoard...");
    if (client.connect("ESP32_INTRUSION", accessToken, NULL)) {
      Serial.println("Connected");
      mqttPublish("SYSTEM CONNECTED");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
  pinMode(REED_PIN, INPUT_PULLUP);
  pinMode(SHOCK_PIN, INPUT_PULLDOWN);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(SHOCK_PIN), shockISR, RISING);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  SPI.begin();
  rfid.PCD_Init();

  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  connectMQTT();

  showOLED("SYSTEM ACTIVE");
  mqttPublish("SYSTEM ACTIVE");
}

/* ================= LOOP ================= */
void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) connectMQTT();
    client.loop();
  }

  /* ---- AUTH SLEEP TIMER ---- */
  if (systemSleep && countdown > 0 && millis() - lastTick >= 1000) {
    lastTick = millis();
    countdown--;
    showOLED("AUTHORISED", "UID: " + activeUID, "TIME: " + String(countdown));
  }

  if (systemSleep && countdown == 0) {
    systemSleep = false;
    activeUID = "";
    showOLED("SYSTEM ACTIVE");
    mqttPublish("SYSTEM ACTIVE");
  }

  /* ---- SENSOR CHECK ---- */
  if (!systemSleep) {
    if (digitalRead(IR_PIN) == LOW)
      triggerAlarm("IR INTRUDER DETECTED");

    if (digitalRead(REED_PIN) == LOW)
      triggerAlarm("DOOR OPEN DETECTED");

    if (shockDetected) {
      shockDetected = false;
      triggerAlarm("TAMPER / SHOCK DETECTED");
      delay(300);
    }
  }

  /* ---- RFID ---- */
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {

    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++)
      uid += String(rfid.uid.uidByte[i], HEX);

    uid.toUpperCase();

    if (uid == authorizedUID1 || uid == authorizedUID2) {
      systemSleep = true;
      countdown = 20;
      lastTick = millis();
      activeUID = uid;

      showOLED("ACCESS GRANTED", "UID: " + uid);
      mqttPublish("AUTHORIZED ACCESS");

      digitalWrite(GREEN_LED, HIGH);
      delay(3000);
      digitalWrite(GREEN_LED, LOW);
    } else {
      showOLED("ACCESS DENIED", "UID: " + uid);
      mqttPublish("UNAUTHORIZED ACCESS");

      digitalWrite(RED_LED, HIGH);
      delay(3000);
      digitalWrite(RED_LED, LOW);
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
