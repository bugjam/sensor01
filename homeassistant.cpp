#include <WiFiS3.h>
#include <ArduinoMqttClient.h>

#include "homeassistant.h"

// 2 minutes
#define RETRY_INTERVAL 120000

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

HaConnection::HaConnection(const char* wifiSsid, const char* wifiPassword, 
                           const char* broker, int port, const char* mqttUser, const char* mqttPassword) {
  _wifiSsid = wifiSsid;
  _wifiPassword = wifiPassword;
  _mqttBroker = broker;
  _mqttPort = port;
  _mqttUser = mqttUser;
  _mqttPassword = mqttPassword;
}

bool HaConnection::isConnected() {
  return _wifiConnected && _mqttConnected;
}

void HaConnection::connect() {
  _wifiConnected = WiFi.status() == WL_CONNECTED;
  if(!_wifiConnected) {
    log("Connecting to wifi");
    int status = WiFi.begin(_wifiSsid,_wifiPassword);
    if(status == WL_CONNECTED) {
      _wifiConnected = 1;
      log("Wifi connection established");
    } else {
      _wifiConnected = 0;
      log("Wifi connection failed");
    }
  }

  if(_wifiConnected && !_mqttConnected) {
    log("Connecting to MQTT broker");
    mqttClient.setUsernamePassword(_mqttUser, _mqttPassword);
    if(mqttClient.connect(_mqttBroker, _mqttPort)) {
      _mqttConnected = 1;
      log("MQTT connection established");
    } else {
      _mqttConnected = 0;
      log("MQTT connection failed");
    }
  }
}

void HaConnection::poll() {
  if(isConnected()) {
    mqttClient.poll();
  } else {
    long currentMillis = millis();
    if(currentMillis - _lastRetry > RETRY_INTERVAL) {
      _lastRetry = currentMillis;
      connect();
    }
  }
}

void HaConnection::log(const char* message) {
  Serial.println(message);
  if(logger) {
    (*logger)(message);
  }
}

void HaConnection::send(HaTemperatureSensor* sensor, float v) {
  char buf[1024];
  if(isConnected()) {
    mqttClient.beginMessage(sensor->stateTopic);
    mqttClient.print(sensor->stateMessage(buf, v));
    mqttClient.endMessage();
  }
}

void HaConnection::send(HaBinarySensor* sensor, int v) {
  char buf[1024];
  if(isConnected()) {
    mqttClient.beginMessage(sensor->stateTopic);
    mqttClient.print(sensor->stateMessage(buf, v));
    mqttClient.endMessage();
  }
}

void HaConnection::enrol(HaSensor* sensor) {
  char buf[1024];
  if(isConnected()) {
    mqttClient.beginMessage(sensor->configTopic, true);
    mqttClient.print(sensor->configMessage(buf));
    mqttClient.endMessage();
  }
}

HaSensor::HaSensor(const char* name, const char* unique_id) {
  _name = name;
  _id = unique_id;
}

char* HaBinarySensor::stateMessage(char* buf, int value) {
  if(value) {
    strcpy(buf, "ON");
  } else {
    strcpy(buf, "OFF");
  }
  return buf;
}

HaTemperatureSensor::HaTemperatureSensor(const char* name, const char* unique_id) : HaSensor(name, unique_id) {
  sprintf(configTopic, "homeassistant/sensor/%s/config", unique_id);
  sprintf(stateTopic, "homeassistant/sensor/%s/state", unique_id);
}

char* HaTemperatureSensor::configMessage(char* buf) {
  sprintf(buf, 
    "{ \"name\":\"%s\","
    "\"dev_cla\":\"temperature\","
    "\"stat_t\":\"%s\","
    "\"uniq_id\":\"%s\","
    "\"exp_aft\":3600,"
    "\"val_tpl\":\"{{ value_json.t}}\","
    "\"dev\":{\"ids\":[\"S_%s\"],\"via_device\":\"Sensor01\"},"
    "\"unit_of_meas\":\"°C\"}", _name, stateTopic, _id, _name);
  return buf;
}

char* HaTemperatureSensor::stateMessage(char* buf, float t) {
  sprintf(buf, "{ \"t\": %.1f }", t);
  return buf;
}

HaBinarySensor::HaBinarySensor(const char* name, const char* unique_id, const char* device_class) : HaSensor(name, unique_id) {
  _device_class = device_class;
  sprintf(configTopic, "homeassistant/binary_sensor/%s/config", unique_id);
  sprintf(stateTopic, "homeassistant/binary_sensor/%s/state", unique_id);
}
const char *HaBinarySensor::DOOR = "door";

char* HaBinarySensor::configMessage(char* buf) {
  sprintf(buf, 
    "{ \"name\":\"%s\","
    "\"dev_cla\":\"%s\","
    "\"stat_t\":\"%s\","
    "\"uniq_id\":\"%s\","
    "\"exp_aft\":150,"
    "\"dev\":{\"ids\":[\"S_%s\"],\"via_device\":\"Sensor01\"}}", 
    _name, _device_class, stateTopic, _id, _name);
  return buf;
}
