/* Sensor01 collects sensor data from external sensors and sends it to HomeAssistant via MQTT
 * Author: Tore Green
 */

// disable alarm support in DallasTemperature
#define REQUIREALARMS false
#include <DallasTemperature.h>
#include "display.h"
#include "settings.h"
#include "homeassistant.h"

#define ONEWIRE_PIN 2
#define BINARY_PIN 3
#define SWITCH_PIN 4

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);
Display display(2);
HaConnection haConnection(WIFI_SSID, WIFI_PASS, MQTT_BROKER, MQTT_PORT, MQTT_USER, MQTT_PASS);
HaTemperatureSensor stue("Stue", "stue_temperature");
HaTemperatureSensor have("Have", "have_temperature");
HaBinarySensor havedoer("Havedør", "havedor_binary", HaBinarySensor::DOOR);

const uint8_t sensorIndexHave = 0;
const uint8_t sensorIndexStue = 1;

unsigned long temperatureLastSent = 0L;
const unsigned long TEMPERATURE_SEND_INTERVAL = 60000;

unsigned long binaryLastSent = 0L;
PinStatus binaryLatestValue = LOW;
const unsigned long BINARY_SEND_INTERVAL = 60000;

PinStatus switchLatestValue = LOW;

void setup() {
  Serial.begin(9600);
  display.begin();
  display.display("Hello, world!   ");
  haConnection.logger = &showMessage;
  haConnection.connect();
  haConnection.enrol(&stue);
  haConnection.enrol(&have);
  haConnection.enrol(&havedoer);
  sensors.begin();
  pinMode(BINARY_PIN, INPUT_PULLUP);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  delay(500);
  display.clear();
}

void loop() {
  display.poll();
  haConnection.poll();
  unsigned long m = millis();
  if(m > temperatureLastSent+TEMPERATURE_SEND_INTERVAL) {
    temperatureLastSent = m;
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(sensorIndexHave);
    if(t == DEVICE_DISCONNECTED_C) {
      display.display("???");
    } else {
      display.display(t, 0);
      haConnection.send(&have, t);
    }

    t = sensors.getTempCByIndex(sensorIndexStue);
    if(t != DEVICE_DISCONNECTED_C) {
      display.display(t, 1);
      haConnection.send(&stue, t);
    }
  }

  PinStatus binaryValue = digitalRead(BINARY_PIN);
  m = millis();
  if(binaryValue != binaryLatestValue || m > binaryLastSent+BINARY_SEND_INTERVAL) {
    haConnection.send(&havedoer, binaryValue);
    binaryLatestValue = binaryValue;
    binaryLastSent = m;
  }

  PinStatus switchValue = digitalRead(SWITCH_PIN);
  if(switchValue == LOW && switchLatestValue == HIGH) {
    display.nextMode();
  }
  switchLatestValue = switchValue;
}

void showMessage(const char* message) {
  char s[strlen(message)+4];
  sprintf(s, "%s   ", message);
  display.display(s);
}