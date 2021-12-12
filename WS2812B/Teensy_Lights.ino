 /*
 *     USB MIDI to WS2812B by mat1jaczczcyyyyy
 *     ----------------------------------
 */

/*  
 * LED Strip definition
 * --------------------
 */

const uint16_t _nLED = 300;  // Number of LEDs
const uint16_t BRIGHTNESS = 100; //Brightness of Lightgrid
const byte _dLED = 5;   // Data pin

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel _LED = Adafruit_NeoPixel(_nLED, _dLED, NEO_GRB + NEO_KHZ800);

/*
 * Color Palette
 * Generate with retinaConverter.py (Retina 2.0+ Palette)
 * ------------------------------------------------------
 */

const byte _r[128] = {0, 28, 125, 255, 255, 255, 85, 24, 255, 255, 85, 36, 255, 255, 85, 24, 134, 81, 28, 16, 73, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 134, 81, 24, 12, 255, 255, 85, 24, 255, 255, 85, 32, 255, 150, 117, 65, 0, 0, 0, 0, 0, 36, 125, 28, 255, 186, 174, 97, 12, 0, 0, 0, 61, 121, 174, 61, 255, 134, 113, 0, 57, 85, 53, 89, 49, 134, 210, 255, 255, 182, 142, 130, 57, 16, 12, 20, 20, 101, 166, 219, 215, 255, 158, 101, 28, 219, 125, 154, 142, 61, 113, 223, 158, 53, 24, 4, 182, 61, 178, 73};
const byte _g[128] = {0, 28, 125, 255, 73, 0, 0, 0, 186, 81, 28, 24, 255, 255, 85, 24, 255, 255, 85, 40, 255, 255, 85, 24, 255, 255, 85, 24, 255, 255, 85, 28, 255, 255, 85, 24, 194, 166, 65, 12, 134, 85, 28, 4, 73, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 20, 53, 81, 97, 57, 85, 81, 0, 69, 0, 125, 28, 0, 255, 235, 255, 138, 255, 166, 40, 0, 0, 24, 32, 73, 223, 255, 255, 255, 255, 255, 138, 81, 125, 28, 0, 125, 174, 255, 89, 40, 73, 77, 20, 28, 57, 0, 81, 105, 223, 223, 178, 28, 255, 255, 150, 101, 61, 113, 255, 0, 0, 206, 65, 174, 49, 93, 20};
const byte _b[128] = {0, 28, 125, 255, 73, 0, 0, 0, 105, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 73, 0, 0, 0, 93, 24, 12, 0, 134, 85, 28, 16, 182, 150, 53, 16, 255, 255, 81, 24, 255, 255, 85, 24, 255, 255, 85, 24, 255, 255, 97, 45, 255, 255, 85, 24, 134, 81, 28, 16, 0, 0, 0, 0, 0, 53, 125, 255, 77, 202, 125, 28, 0, 45, 4, 8, 0, 134, 255, 255, 255, 255, 121, 0, 0, 4, 20, 0, 36, 109, 202, 255, 194, 231, 255, 89, 0, 0, 0, 4, 0, 12, 53, 40, 89, 24, 8, 61, 24, 36, 45, 12, 45, 105, 186, 255, 255, 61, 113, 255, 0, 0, 0, 0, 0, 0, 0, 0};

/*
 * MIDI handler
 * ------------
 */

const byte _pStart = 0;  // First note in array
bool update = false;
int counter = 0;
int dif = 0;

void noteOn(byte ch, byte p, byte v) {
  _LED.setPixelColor(p + ((ch-1) * 128 - dif) - _pStart, _r[v], _g[v], _b[v]);
  update = true;
}

void noteOff(byte ch, byte p, byte v) {
  _LED.setPixelColor(p + ((ch-1) * 128 - dif) - _pStart, 0, 0, 0);
  update = true;
}

void sysEx(byte *data, unsigned int length) {
  if (length == 6) {
    //_LED.setPixelColor(*(data+1) - _pStart, *(data+2), *(data+3), *(data+4));
    update = true;
  }
}

/*
 * Initialize LED Strip and USB MIDI
 * ---------------------------------
 */

void setup() {
  randomSeed(analogRead(14));
  _LED.begin();
  _LED.setBrightness(BRIGHTNESS);
  _LED.clear();
  int r = random(0, 255);
  int g = random(0, 255 - r);
  int b = 255 - r - g; 

  meteorRain(r, g, b, 20, 60, true, 1, 1);
  
  usbMIDI.setHandleNoteOn(noteOn);
  usbMIDI.setHandleNoteOff(noteOff);
  usbMIDI.setHandleSystemExclusive(sysEx);
} 

/* 
 * Read MIDI and update Strip
 * --------------------------
 */
void loop() {
  while (usbMIDI.read());
  if (update) {
    _LED.show();
    counter ++; 
    Serial.println(counter);
    update = false;
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay, int stepSize) {  
  _LED.clear();
 
  for(int i = 0; i < (_nLED+_nLED); i += stepSize) {
   
   
    // fade brightness all LEDs one step
    for(int j=0; j<_nLED; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j < _nLED) && (i-j>=0) ) {
        _LED.setPixelColor(i-j, red, green, blue);
      }
    }
   
    _LED.show();
    delay(SpeedDelay);
  }
}


void fadeToBlack(int ledNo, byte fadeValue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
   
    oldColor = _LED.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
   
    _LED.setPixelColor(ledNo, r,g,b);
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[ledNo].fadeToBlackBy( fadeValue );
 #endif  
}
