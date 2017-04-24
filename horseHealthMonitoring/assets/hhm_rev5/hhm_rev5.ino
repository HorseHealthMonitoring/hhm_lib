#include "Adafruit_FONA.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "OneWire.h"
#include "DallasTemperature.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define ONE_WIRE_BUS 5

//Setup for temperature interface.
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int wakePin = 5;
int temp;

// this is a large buffer for replies
char replyBuffer[255];
char *SMSnumber = "6015962842";

//Value for watchdog timer interrupt.
volatile int f_wdt = 1;
volatile int seconds = 0;
volatile int minutes = 30;
volatile int hours = 0;
volatile int timerCounter = 0;

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
  while (!Serial);
  Serial.begin(115200);
  setupGsm();
  setupWdt();
  lastTime = micros();
  sensors.begin();
}

void loop()
{
  if(f_wdt == 1)
  {
    wdt_disable();
    int interval = 0;
    int nowFlag = 0;
    int8_t smsnum = fona.getNumSMS();
    if (smsnum > 0)
    {
      delay(100);
      readSMS();
      Serial.println(replyBuffer);
      if (strcmp(replyBuffer, "NOW") == 0)
      {
        nowFlag = 1;
        Serial.println(F("Got a now."));
      }
      else if (strcmp(replyBuffer, "CN") == 0)
      {
        Serial.println(F("Change number command."));
        fona.getSMSSender(smsnum, replyBuffer, 250);
        SMSnumber = replyBuffer;
        char *newNumber;
        for (int k = 0; k < strlen(SMSnumber) + 1; k++)
        {
          Serial.println(SMSnumber[k]);
          if (k > 0)
          {
            newNumber = appendCharToCharArray(newNumber, SMSnumber[k]);
          }
          else
          {
            continue;
          }
        }
        SMSnumber = newNumber;
        sendNumberChangeAck();
      }
      else if (strcmp(replyBuffer, "BAT") == 0)
      {
        Serial.println(F("Check battery command."));
        sendBatteryAck();
      }
      else if (strcmp(replyBuffer, "CI") == 0)
      {
        Serial.println(F("Change interval command."));
        changeInterval();
      }
      else if (strcmp(replyBuffer, "HELP") == 0)
      {
        Serial.println(F("Text help info command."));
        sendHelp();
      }
      else
      {
        Serial.println(F("Text 'HELP'."));
        askHelp();
      }

      deleteAllSMS();
    }

    interval = ((hours*60*60) + (minutes*60) + (seconds))/8;
    Serial.println(interval);
    Serial.println(timerCounter);
    if ((timerCounter >= interval) || (nowFlag == 1))
    {
      //Turn off watchdog so we can do stuff.
      average = 0;
      grabCount = 0;
      grabber = 1000;
      
      for (int i = 0; i < 30001; i++)
      {
        delay(1);
        pollPulse();
        if (i == grabber)
        {
          if ((BPM < 100) && (BPM > 50))
          {
          average += BPM;
          grabCount += 1;
          }
          grabber += 1000;
        }
      }
      average = average/grabCount;
      Serial.println(average);
      
      getTemp();
      Serial.println(F("Sending..."));
      sendMetrics();
      
      //Reset timer.
      timerCounter = 0;
      //Turn watchdog back on.
    }
    setupWdt();
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

void pollPulse()
{
  thisTime = micros();
  if (thisTime - lastTime > 2000)
  {
    lastTime = thisTime;
    getPulse();
  }
}
void getTemp()
{
  sensors.requestTemperatures();
  temp = sensors.getTempFByIndex(0);
  Serial.println(temp);
  delay(1);
}

void sendMetrics()
{
  char stack[128];
  memset(&stack, 0, sizeof(stack));
  sprintf(stack, "Temperature: %d, Heartrate: %d. Next update in: %d hours, %d minutes, %d seconds.", temp, average, hours, minutes, seconds);
  fona.sendSMS(SMSnumber, stack);
}

void sendNumberChangeAck()
{
  char stack[128];
  memset(&stack, 0, sizeof(stack));
  sprintf(stack, "Successfully changed receiving phone number to: %s.", SMSnumber);
  fona.sendSMS(SMSnumber, stack);
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

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

void readSMS()
{
    int8_t smsnum = fona.getNumSMS();
    uint16_t smslen;
    int8_t smsn = 1;

    for ( ; smsn <= smsnum; smsn++) {
      Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);
      if (!fona.readSMS(smsn, replyBuffer, 250, &smslen)) {  // pass in buffer and max len!
        Serial.println(F("Failed!"));
        break;
      }
      // if the length is zero, its a special case where the index number is higher
      // so increase the max we'll look at!
      if (smslen == 0) {
        Serial.println(F("[empty slot]"));
        smsnum++;
        continue;
      }

      Serial.print(F("*****"));
      Serial.println(replyBuffer);
      Serial.println(F("*****"));
    }
}

char* appendCharToCharArray(char* array, char a)
{
    size_t len = strlen(array);

    char* ret = new char[len+2];

    strcpy(ret, array);    
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
}

void sendBatteryAck()
{
  char stack[128];
  uint16_t vbat;
  uint16_t pbat;
  fona.getBattVoltage(&vbat);
  fona.getBattPercent(&pbat);

  memset(&stack, 0, sizeof(stack));
  sprintf(stack, "Battery voltage: %d mV, Battery percentage: %d %.", vbat, pbat);
  fona.sendSMS(SMSnumber, stack);
}

void changeInterval()
{
  char stack[128];
  int8_t smsnum = fona.getNumSMS();
  deleteAllSMS();
  sendIntervalChangeAck();
  while (fona.getNumSMS() < 1)
  {
    Serial.println(F("Waiting..."));
  }
  readSMS();
  int  secTemp = 0;
  int minTemp = 0;
  int hourTemp = 0;
  Serial.println(replyBuffer);
  hourTemp = atoi(replyBuffer + 1);
  minTemp = atoi(replyBuffer + 4);
  secTemp = atoi(replyBuffer + 7);
  seconds = secTemp;
  minutes = minTemp;
  hours = hourTemp;
  memset(&stack, 0, sizeof(stack));
  sprintf(stack, "Interval changed to: hours: %d, minutes: %d, seconds: %d.", hours, minutes, seconds);
  fona.sendSMS(SMSnumber, stack);
}

void sendIntervalChangeAck()
{
  char stack[128];
  memset(&stack, 0, sizeof(stack));
  sprintf(stack, "Changing interval. Please send in format: H--M--S--.");
  fona.sendSMS(SMSnumber, stack);
}

void deleteAllSMS()
{
  int8_t smsnum = fona.getNumSMS();
  for(int l = 1; l < (smsnum + 1); l++)
  {
    fona.deleteSMS(l);
  }
}

void askHelp()
{
  char stack[128];
  memset(&stack, 0, sizeof(stack));
  sprintf(stack, "For help, send a 'HELP' text.");
  fona.sendSMS(SMSnumber, stack);
}

void sendHelp()
{
  char stack[128];
  memset(&stack, 0, sizeof(stack));
  sprintf(stack, "Commands: NOW (Get current status), CI (Change Interval), BAT (Battery Status), CN (Change number).");
  fona.sendSMS(SMSnumber, stack);
}
