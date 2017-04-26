
#ifndef __8BITDISPLAY_H__
#define __8BITDISPLAY_H__
#include "Particle.h"

class X8bitDisplay {
public:
    void begin(int dataPin, int clockPin, int latchPin);
    void clear(void);
    void write(String text);
    void update(void);
    void setUpdateRate(int ms);
private:
    int _latchPin;
    int _clockPin;
    int _dataPin;
    int _updateRate;
    int _lastUpdateMillis;
    String _currentText;
    void writeByte(byte b);
    byte convertByte(char c);
    void enableWrite(void);
    void disableWrite(void);
};

#endif
