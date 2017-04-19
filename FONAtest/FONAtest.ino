#include "Adafruit_FONA.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define FONA_RX 10
#define FONA_TX 9
#define FONA_RST 4
#define LED_PIN 8
#define ONE_MINUTE (1 * 10 * 1000UL)

// this is a large buffer for replies
char replybuffer[255];
char *SMSnumber = "6015962842";
char stack[128] = {'c'};

//Value for watchdog timer interrupt.
volatile int f_wdt = 1;
int seconds = 8;
int minutes = 0;
int hours = 0;
int interval = ((hours*60*60) + (minutes*60) + (seconds))/8;
int timerCounter = 0;

//Setup for pulse sensor.
volatile int Signal;                // holds the incoming raw data
int pulsePin = 0;                   //Pin to read at analog 0 for the pulse.
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
unsigned long lastTime; // used to time the Pulse Sensor samples
unsigned long thisTime; // used to time the Pulse Sensor samples
int grabber;
int average;
int grabCount;

//ISR for watchdog timer.
ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
    timerCounter++;
  }
  else
  {
    Serial.println("WDT Overrun!!!");
  }
}

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines 
// and uncomment the HardwareSerial line
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
//HardwareSerial *fonaSerial = &Serial;

// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  while (!Serial);
  Serial.begin(115200);
  setupGsm();
  setupWdt();
  interruptSetup();
  digitalWrite(LED_PIN, HIGH);
}

void loop()
{
  if(f_wdt == 1)
  {
    if (timerCounter == interval)
    {
      wdt_disable();
      //Turn off watchdog so we can do stuff.

      getPulse();
      
      //Reset timer.
      timerCounter = 0;
      //Turn watchdog back on.
      setupWdt();
    }
    /* Don't forget to clear the flag. */
    f_wdt = 0;
    
    /* Re-enter sleep mode. */
    enterSleep();
  }
  else
  {
    /* Do nothing. */
  }
}

void setupGsm()
{
  fonaSerial->begin(4800);
  while (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    delay(1000);
  }
}

void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}

void getPulse()
{
  grabber = 1000;
  average = 0;
  grabCount = 0;
  for (int i = 0; i < 20001; i++)
  {
    delay(1);
    if (i == grabber)
    {
      if ((BPM < 100) && (BPM > 50))
      {
        average += BPM;
        grabCount += 1;
      }
      grabber += i;
    }
  }
  Serial.println(average/grabCount);
  delay(10);
}

void setupWdt()
{
  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
}

