#include "display.h"

Display::Display(int slots) {
  _slots = slots;
  _values = new float[slots];
  _mode = 0;
  _displaySlot = 0;
}

void Display::begin() {
  matrix.begin();
}

void Display::display(char message[]) {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(70);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(message);
  if(strlen(message) > 3) {
    matrix.endText(SCROLL_LEFT);
  } else {
    matrix.endText();
  }
  matrix.endDraw();
}

void Display::display(float f) {
  if(f > -10.0) {
    int i = f >= 0.0 ? floor(f) : ceil(f);
    int d = round((f-i)*10);
    i = i % 100;
    d = abs(d) % 10;
    char int_part[3];
    char decimal[2];
    if(i == 0 && f < 0.0) {
      strcpy(int_part,"-0");
    } else {
      sprintf(int_part,"%2i", i);
    }
    sprintf(decimal, "%i", d);

    // display integer part
    matrix.beginDraw();
    matrix.stroke(0xFFFFFFFF);
    matrix.textFont(Font_4x6);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.print(int_part);
    matrix.endText();

    // decimal point
    matrix.point(8,6);
    matrix.point(7,6);
    matrix.point(7,7);

    // decimal
    matrix.beginText(9, 1, 0xFFFFFF);
    matrix.print(decimal);
    matrix.endText();

    matrix.endDraw();
  } else {
    // display without decimal for f <= -10.0
    int i = round(f);
    i = i % 100;
    char int_part[3];
    sprintf(int_part,"%03i", i);
    matrix.beginDraw();
    matrix.stroke(0xFFFFFFFF);
    matrix.textFont(Font_4x6);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.print(int_part);
    matrix.endText();
    matrix.endDraw();
  }
}

void Display::display(float v, int s) {
  _values[s] = v;
  if(s == _displaySlot) {
    display(v);
  }
}

void Display::clear() {
  matrix.beginDraw();
  matrix.clear();
  matrix.endDraw();
}

int Display::nextMode() {
  if(++_mode >= _slots) {
    _mode = MODE_ROTATE;
  }
  _slotLastChanged = millis();
  switch(_mode) {
    case MODE_BLACK:
      _displaySlot = MODE_BLACK;
      clear();
      break;
    case MODE_ROTATE:
      _displaySlot = 0;
      display(_values[_displaySlot]);
      break;
    default:
      _displaySlot = _mode;
      display(_values[_displaySlot]);
      break;
  }
  return _mode;
}

void Display::poll() {
  unsigned long m;
  if(_mode == MODE_ROTATE) {
    m = millis();
    if(m > (_slotLastChanged + INTERVAL)) {
      if(++_displaySlot >= _slots) {
        _displaySlot = 0;
      }
      display(_values[_displaySlot]);
      _slotLastChanged = m;
    }
  }  
}