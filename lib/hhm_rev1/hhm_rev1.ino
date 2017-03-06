/*
 FILE NAME: hhm_rev1
 AUTHORS: Austin Heath, Hussein Al-Sabbahi, Jeremy Brown, Cassandra Sasser
 DESCRIPTION: First revision of the Horse Health Monitoring system designed to receive GSM commands, take temperature of a horse and take the pulse rate of a horse.
 
 THIS CODE IS STILL IN PROGRESS!
 */

//INCLUDES
#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>

//DEFINES
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

//GLOBALS
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;

  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while (Serial.available()) {
      char c =  Serial.read();

      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;

        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }

    if (timeoutvalid && timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

void setup()
{
  while (!Serial);
  Serial.begin(115200);
  fonaSerial->begin(4800);
  char sendto[21], message[141];
  flushSerial();
  Serial.print(F("Send to #"));
  readline(sendto, 20);
  Serial.println(sendto);
  Serial.print(F("Type out one-line message (140 char): "));
  readline(message, 140);
  Serial.println(message);
  if (!fona.sendSMS(sendto, message)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("Sent!"));
  }
}

void loop()
{
  while(1)
  {
    Serial.print(F("working"));
  }
}

