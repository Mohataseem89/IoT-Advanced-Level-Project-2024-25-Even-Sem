// Include Blynk Template Info
#define BLYNK_TEMPLATE_ID "TMPL3ovHK2NTL"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// Blynk credentials
char auth[] = "8OJVe0LBnXQTiA61u6A46IF-nqf7sfEm";
char ssid[] = "TP-LINK_E3869A";
char pass[] = "11111111";

DHT dht(D3, DHT11);
BlynkTimer timer;
bool pirbutton = 0;

// Define component pins
#define Buzzer D0
#define MQ2 A0
#define trig D4
#define echo D5
#define PIR D6
#define relay1 D7
#define relay2 D8

// Get PIR enable/disable from app
BLYNK_WRITE(V0) {
  pirbutton = param.asInt();
}

void setup() {
  Serial.begin(9600);

  pinMode(Buzzer, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  // Call the functions periodically
  timer.setInterval(100L, gassensor);
  timer.setInterval(2000L, DHT11sensor);
  timer.setInterval(100L, pirsensor);
  timer.setInterval(100L, ultrasonic);
}

// MQ2 gas sensor
void gassensor() {
  int value = analogRead(MQ2);
  Serial.println(value);
  value = map(value, 0, 1024, 0, 100);
  if (value <= 55) {
    digitalWrite(Buzzer, LOW);
  } else {
    Blynk.logEvent("gas_alert", "Warning! Gas leak detected"); // Make sure to create 'gas_alert' event
    digitalWrite(Buzzer, HIGH);
  }
  Blynk.virtualWrite(V1, value);
}

// DHT11 sensor
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
}

// PIR sensor
void pirsensor() {
  bool value = digitalRead(PIR);
  if (pirbutton == 1) {
    if (value == 0) {
      digitalWrite(Buzzer, LOW);
    } else {
      Blynk.logEvent("pir_alert", "Warning! Please check your security system"); // Create 'pir_alert' event
      digitalWrite(Buzzer, HIGH);
    }
  }
}

// Ultrasonic sensor
void ultrasonic() {
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  long cm = t / 29 / 2;
  Blynk.virtualWrite(V4, cm);
}

// Relay 1 control
BLYNK_WRITE(V5) {
  bool RelayOne = param.asInt();
  digitalWrite(relay1, RelayOne ? LOW : HIGH);
}

// Relay 2 control
BLYNK_WRITE(V6) {
  bool RelayTwo = param.asInt();
  digitalWrite(relay2, RelayTwo ? LOW : HIGH);
}

void loop() {
  Blynk.run();
  timer.run();
}
