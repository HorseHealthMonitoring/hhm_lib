/* Arduino DS18B20 temp sensor tutorial
   More info: http://www.ardumotive.com/how-to-use-the-ds18b20-temperature-sensor-en.html
   Date: 19/6/2015 // www.ardumotive.com */

//Include libraries
#include "OneWire.h"
#include "DallasTemperature.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int sleepPin = 12;          // active LOW, ground this pin momentary to sleep
int interruptPin = 10;      // LED to show the action of a interrupt
int wakePin = 2;            // active LOW, ground this pin momentary to wake up
int sleepStatus = 0;        // variable to store a request for sleep

void setup(void)
{
/*Sleep Stuff I added starts here, comment out if there is a problem
  pinMode(interruptPin, OUTPUT);   //
  pinMode(sleepPin, INPUT);        // sets the digital pin as input
  pinMode(wakePin, INPUT);

  attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                      // wakeUpNow when pin 2 gets LOW
  Sleep Stuff I added ends here, comment out if there is a */
  
  Serial.begin(9600); //Begin serial communication
  Serial.println("Arduino Digital Temperature // Serial Monitor Version"); //Print a message
  sensors.begin();

  
}


void sleepNow()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep modus.
     *
     * In the avr/sleep.h file, the call names of these sleep modus are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible,
     * so we choose the according sleep modus: SLEEP_MODE_PWR_DOWN
     *
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();              // enables the sleep bit in the mcucr register
                                 // so sleep is possible. just a safety pin
 
    /* Now is time to enable a interrupt. we do it here so an
     * accidentally pushed interrupt button doesn't interrupt
     * our running program. if you want to be able to run
     * interrupt code besides the sleep function, place it in
     * setup() for example.
     *
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.  
     *
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */
 
    attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();                // here the device is actually put to sleep!!
                                 //
 
    sleep_disable();             // first thing after waking from sleep:
                                 // disable sleep...
    detachInterrupt(0);          // disables interrupt 0 on pin 2 so the
                                 // wakeUpNow code will not be executed
                                 // during normal running time.
    delay(1000);                 // wat 2 sec. so humans can notice the
                                 // interrupt.
                                 // LED to show the interrupt is handled
    digitalWrite (interruptPin, LOW);      // turn off the interrupt LED
 
}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
  //execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  digitalWrite(interruptPin, HIGH);
}

void loop(void)
{ 
  // Send the command to get temperatures
  sensors.requestTemperatures();  
  Serial.print("Temperature is: ");
  Serial.println(sensors.getTempFByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  //Update value every 1 sec.
  delay(1000);
}
