#ifndef DISPLAY_H
#define DISPLAY_H

#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>

#define INTERVAL 20000L

class Display {
  public:
    Display(int slots);
    void begin();
    void display(char message[]);
    void display(float f);
    void display(float f, int slot);
    void clear();
    void poll();
    int nextMode();
    const static int MODE_ROTATE = -2;
    const static int MODE_BLACK = -1;
  private:
    ArduinoLEDMatrix matrix;
    int _mode;
    int _displaySlot;
    int _slots;
    float* _values;
    unsigned long _slotLastChanged;
};

#endif