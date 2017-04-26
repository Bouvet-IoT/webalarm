#include "x8bitDisplay.h"
#include "Particle.h"

//The Arrays Tab and Taf contain numbers and letters encoded as HEX which tells the LED Module which segments to light.
//
//For example the first element of Tab =0xC0 which is the number 0.
//
// 0=0xC0
// 0xC0=11000000
//The zeros and ones represent the light(0's) segments and the off(1's) segments from the 7 segment display.

// P G F E D C B A
//   A
//   --
// F|__|B
// E|G |C
//   --   .P
//   D
//So in this case P and G will be off. which makes 0

const byte Tab[]={
    0xc0, // 0
    0xf9, // 1
    0xa4, // 2
    0xb0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xf8, // 7
    0x80, // 8
    0x90, // 9
    0xff  // ALL OFF
};

const byte Taf[]={
    0xA0, // a
    0x83, // b
    0xa7, // c
    0xa1, // d
    0x86, // e
    0x8e, // f
    0x82, // g  evt: c2
    0x8b, // h
    0xf9, // i
    0xe1, // j
    0x89, // k
    0xc7, // l
    0xaa, // m
    0xc8, // n
    0xa3, // o
    0x8c, // p
    0x98, // q
    0xce, // r
    0x92, // s
    0x87, // t
    0xc1, // u
    0xe3, // v
    0xd5, // w
    0xb6, // x
    0x91, // y
    0xa4  // z
};

const byte SPACE = 0xff;
const byte CLEAR = 0xff;
const byte DOT = 0x7f;
const byte COMMA = 0x7f;
const byte DASH = 0xbf;
const byte UNDERSCORE = 0xf7;
const byte EXCLAMATIONMARK = 0x7d;

void X8bitDisplay::begin(int dataPin, int clockPin, int latchPin){
    _latchPin = latchPin;
    _dataPin = dataPin;
    _clockPin = clockPin;
    
    pinMode(_latchPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);  
    pinMode(_clockPin, OUTPUT);
    
    _updateRate = 100;
    
    clear();
}

void X8bitDisplay::clear(void) {
    for(int i = 0; i < 8; i++) {
        writeByte(0xff);
    }
}

void X8bitDisplay::writeByte(byte b) {
    shiftOut(_dataPin, _clockPin, MSBFIRST, b);
}

void X8bitDisplay::update(void) {
    if(millis() - _lastUpdateMillis < _updateRate) {
        return;
    }
    
    _lastUpdateMillis = millis();

    char bitToSet = 0;
    byte b = 0x0;
    String bufText = _currentText;
    byte nextChar = 0x0;
    
    String tmpCpy = bufText;
    tmpCpy.replace(".", "").replace(",", "");
    for(int i = tmpCpy.length(); i < 8; i++) {
        bufText = " " + bufText;
    }
    
    enableWrite();
    
    for(int i = bufText.length() - 1; i >= 0; i--){
        bitToSet = bufText.charAt(i);
        
        b = convertByte(bitToSet);
        
        if(b == DOT || b == COMMA)
            continue;
            
        nextChar = convertByte(bufText.charAt(i + 1));
        if(nextChar == DOT || nextChar == COMMA)    
            b = b & DOT;
        
        if(b != 0){
            writeByte(b);
        }
    }
    
    disableWrite();
}

void X8bitDisplay::write(String text) {
    _currentText = text;
}

void X8bitDisplay::setUpdateRate(int ms) {
    _updateRate = ms;
}

byte X8bitDisplay::convertByte(char c) {
    byte b = 0xff;
    
    //test for "space"
    if (c == 32){
        b = SPACE;
    }
    //test for "."
    else if (c == 44){
        b = COMMA;
    }
    else if (c == 45) {
        b = DASH;
    }
    //test for "."
    else if (c == 46){
        b = DOT;
    }
    else if (c == 95) {
        b = UNDERSCORE;
    }
    else if (c == 33) {
        b = EXCLAMATIONMARK;
    }
    //test for numbers
    else if (c <= 57 && c >= 48){
        c = c - 48;
        b = Tab[c];
    }
    //test for letters
    else if ( c <= 90 && c >= 65){
        c= c - 65;
        b = Taf[c];
    }
    //test for letters
    else if ( c <= 122 && c >= 97){
        c= c - 97;
        b = Taf[c];
    }
    
    return b;
}

void X8bitDisplay::enableWrite(void) {
    digitalWrite(_latchPin, LOW);
}

void X8bitDisplay::disableWrite(void) {
    digitalWrite(_latchPin, HIGH);
}









