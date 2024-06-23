#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_NeoPixel.h>
#include <PWMAudio.h>
#include "wav.h"

//Audio setup
const int16_t *start = (const int16_t *)out_raw;
const int16_t *p = start;
unsigned int count = 0;

// Create the PWM audio device on GPIO 29/A0.  
PWMAudio pwm(29);


//er-gah-bled(s) yaaaas
#define NUMPIXELS        1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

Adafruit_ADXL343 accel = Adafruit_ADXL343(12345, &Wire1);


void setup(void)
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Accelerometer Test"); Serial.println("");

    /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);

  /* Display some basic information on this sensor */
  accel.printSensorDetails();
  Serial.println("");

  #if defined(NEOPIXEL_POWER)
    // If this board has a power control pin, we must set it to output and high
    // in order to enable the NeoPixels. We put this in an #if defined so it can
    // be reused for other boards without compilation errors
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, HIGH);
  #endif

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20); // not so bright
  
}

//Has to be a separate fuction to make the callback work 
void transmitAudio()
{
  while (pwm.availableForWrite())
  {  
    if (count >= sizeof(out_raw)) {
      
      pwm.end();
    } 
    else
    {
      pwm.write(*p++);
      count += 2;
    }
  }
}

//config audio and trigger callback
void play() {
   p = start;
   count = 0;
  
  p = start;
  pwm.begin(44100);
    
  pwm.onTransmit(transmitAudio);
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);

  // If someone picks up the thing
  if(event.acceleration.z > 10)
  {
    Serial.println("RED!");
    pixels.fill(0xFF0000);
    pixels.show();
    play();
    delay(5000); //wait after alarm
  } 
  else //chill
  {
    pixels.fill(0x00FF00);
    pixels.show();
    Serial.println("super green.");
  }
  delay(10); // wait to let adxl catch up
}