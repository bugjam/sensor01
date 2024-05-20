# Sensor01

Sensor01 collects sensor data from external sensors and sends it to HomeAssistant via MQTT.
Current configuration includes 2 temperature sensors and 1 door sensor.
Temperature is shown on the Arduino Uno's built-in LED matrix. A push button supports switching between display modes (In, Out, Alternating between in and out, Display off).

## Required libraries:
* [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)
* [ArduinoGraphics](https://www.arduino.cc/reference/en/libraries/arduinographics/)
* [ArduinoMqttClient](https://www.arduino.cc/reference/en/libraries/arduinomqttclient/)