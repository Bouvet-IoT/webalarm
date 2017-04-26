
#ifndef __ALARMCLOCK_H__
#define __ALARMCLOCK_H__
#include "Particle.h"
#include "x8bitDisplay.h"

class AlarmClock {
public:
    bool customTextOnDisplay;
    void enableDisplay(X8bitDisplay* display);
    void enableAlarm(int soundPin);
    void setAlarmTime(int hour, int minute);
    void update(void);
    void snoozeAlarm(void);
    void turnOffAlarm(void);
private:
    X8bitDisplay* _display;
    int _soundPin;
    int _alarmHour;
    int _alarmMinute;
    bool _alarmModeEnabled;
    bool _displayModeEnabled;
    bool _alarmActivated;
    void setSound(bool value);
    void disableAlarmTimes(void);
};

#endif
