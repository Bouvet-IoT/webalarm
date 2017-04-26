#include "Particle.h"
#include "AlarmClock.h"

void AlarmClock::enableDisplay(X8bitDisplay* display) {
    _displayModeEnabled = true;
    _display = display;   
}

void AlarmClock::enableAlarm(int soundPin) {
    _alarmModeEnabled = true;
    _soundPin = soundPin;
    disableAlarmTimes();
    
    pinMode(_soundPin, OUTPUT);
}

void AlarmClock::setAlarmTime(int hour, int minute) {
    _alarmHour = hour;
    _alarmMinute = minute;
}

void AlarmClock::update(void) {
    if(Time.hour() == _alarmHour && Time.minute() == _alarmMinute){
        setSound(true);
    }
    
    if(!customTextOnDisplay && _displayModeEnabled) {
        if(Time.second() % 2 == 0){
            _display->write(Time.format(Time.now(), "%H-%M-%S"));
        }
        else{
            if(_alarmActivated)
                _display->write("Wake up!");
            else
                _display->write(Time.format(Time.now(), "%H %M %S"));
        }
    }
}

void AlarmClock::setSound(bool on) {
    _alarmActivated = on;
    if(on)
        digitalWrite(_soundPin, HIGH);
    else
        digitalWrite(_soundPin, LOW);
}

void AlarmClock::snoozeAlarm(void) {
    if(Time.minute() < 55)
        _alarmMinute = Time.minute() + 5;
    else {
        _alarmHour = Time.hour() + 1;
        _alarmMinute = 00;
    }
    
    setSound(false);
}

void AlarmClock::turnOffAlarm(void) {
    disableAlarmTimes();
    setSound(false);
}

void AlarmClock::disableAlarmTimes(void) {
    _alarmMinute = 99;
    _alarmHour = 99;
}









