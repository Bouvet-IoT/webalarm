// This #include statement was automatically added by the Particle IDE.
#include "AlarmClock.h"

// This #include statement was automatically added by the Particle IDE.
#include "x8bitDisplay.h"

PRODUCT_ID(2991);
PRODUCT_VERSION(1);

// -----------------------------------
// Controlling LEDs over the Internet
// -----------------------------------

// First, let's create our "shorthand" for the pins
// Same as in the Blink an LED example:
// Alarm is D0, led2 is D7

const int led1 = D7;
const int alarm = D7;

//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = D2;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = D1;
//Pin connected to Data in (DS) of 74HC595
const int dataPin = D3;

X8bitDisplay display;
AlarmClock alarmclock;

bool customText = false;
int customTextAt;
int customTextMaxTime = 10000;

// Last time, we only needed to declare pins in the setup function.
// This time, we are also going to register our Particle function

void setup()
{
    Time.zone(+1);
    // Here's the pin configuration, same as last time
    pinMode(led1, OUTPUT);

    // We are also going to declare a Particle.function so that we can turn the LED on and off from the cloud.
    Particle.function("led", ledToggle);
    // This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.
    Particle.function("alarm", alarmToggle);
    Particle.function("display", writeToDisplay);
    Particle.function("snooze", snoozeAlarm);
    Particle.function("setalarm", setAlarmTime);

    // For good measure, let's also make sure both LEDs are off when we start:
    digitalWrite(led1, LOW);
    digitalWrite(alarm, LOW);

    display.begin(dataPin, clockPin, latchPin);
    
    display.write("Hallo! ");
    
    alarmclock.customTextOnDisplay = true;
    customTextAt = millis() + 10000;
    
    alarmclock.enableDisplay(&display);
    alarmclock.enableAlarm(alarm);
    //alarmclock.setAlarmTime(Time.hour(), Time.minute() + 2);
}


// Last time, we wanted to continously blink the LED on and off
// Since we're waiting for input through the cloud this time,
// we don't actually need to put anything in the loop

void loop()
{
    if(customTextAt < millis() - customTextMaxTime) {
        alarmclock.customTextOnDisplay = false;
    }
    
    display.update();
    alarmclock.update();
}

// We're going to have a super cool function now that gets called when a matching API request is sent
// This is the ledToggle function we registered to the "led" Particle.function earlier.
int ledToggle(String command) {
    /* Particle.functions always take a string as an argument and return an integer.
    Since we can pass a string, it means that we can give the program commands on how the function should be used.
    In this case, telling the function "on" will turn the LED on and telling it "off" will turn the LED off.
    Then, the function returns a value to us to let us know what happened.
    In this case, it will return 1 for the LEDs turning on, 0 for the LEDs turning off,
    and -1 if we received a totally bogus command that didn't do anything to the LEDs.
    */

    if (command=="on") {
        digitalWrite(led1,HIGH);
        return 1;
    }
    else if (command=="off") {
        digitalWrite(led1,LOW);
        return 0;
    }
    else if(command == "toggle") {
        if (digitalRead(led1) == LOW) {
            digitalWrite(led1, HIGH);
            return 1;
        }
        else if (digitalRead(led1) == HIGH) {
            digitalWrite(led1, LOW);
            return 2;
        }
    }
    
    return -1;
}

int alarmToggle(String command) {
    
    if (command == "on") {
        digitalWrite(alarm,HIGH);
        return 1;
    }
    else if (command == "off") {
        alarmclock.turnOffAlarm();
        digitalWrite(alarm,LOW);
        return 0;
    }
    else if (command == "toggle") {
        if (digitalRead(alarm) == LOW) {
            digitalWrite(alarm, HIGH);
            return 1;
        }
        else if (digitalRead(alarm) == HIGH) {
            alarmclock.turnOffAlarm();
            digitalWrite(alarm, LOW);
            return 2;
        }
    }
    
    return -1;
}

int writeToDisplay(String text) {
    display.write(text);
    alarmclock.customTextOnDisplay = true;
    customTextAt = millis();
    return 0;
}

int snoozeAlarm(String ignore) {
    alarmclock.snoozeAlarm();
    writeToDisplay("-snooze-");
}

int setAlarmTime(String hhmm) {
    if(hhmm.length() != 4)
        return -1;
    
    int t = hhmm.toInt();
    
    int m = t % 100;
    if(m > 59)
        return -2;
    
    int h = t / 100;
    if(h > 23) 
        return -3;
        
    alarmclock.setAlarmTime(h, m);
    
    return writeToDisplay("alm " + String(h) + String(m));
}
