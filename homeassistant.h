#ifndef HOME_ASSISTANT_H
#define HOME_ASSISTANT_H

class HaSensor {
  public:
    HaSensor(const char* name, const char* unique_id);
    char configTopic[100];
    char stateTopic[100];
    virtual char* configMessage(char* buf) = 0;
  protected:
    const char* _name;
    const char* _id;
};

class HaTemperatureSensor : public HaSensor {
  public:
    HaTemperatureSensor(const char* name, const char* unique_id);
    virtual char* configMessage(char* buf);
    virtual char* stateMessage(char* buf, float value);
};

class HaBinarySensor : public HaSensor {
  public:
    HaBinarySensor(const char* name, const char* unique_id, const char* device_class);
    static const char *DOOR;
    virtual char* configMessage(char* buf);
    virtual char* stateMessage(char* buf, int value);
  protected:
    const char* _device_class;
};

class HaConnection {
  public:
    HaConnection(const char* wifiSsid, const char* wifiPassword, 
                 const char* broker, int port, const char* mqttUser, const char* mqttPassword);
    void connect();
    bool isConnected();
    void poll();
    void enrol(HaSensor* sensor);
    void send(HaBinarySensor* sensor, int value);
    void send(HaTemperatureSensor* sensor, float value);
    void (*logger)(const char* message);
  private:
    void log(const char* message);
    const char* _wifiSsid;
    const char* _wifiPassword;
    const char* _mqttBroker;
    int _mqttPort;
    const char* _mqttUser;
    const char* _mqttPassword;
    int _wifiConnected = 0;
    int _mqttConnected = 0;
    long _lastRetry = 0;
};

#endif