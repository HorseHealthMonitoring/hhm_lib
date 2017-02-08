#ifndef REVF16_H_
#define REVF16_H_

#include "pic24_all.h"
#include "all_generic.h"
#include "esos_pic24_sensor.h"
#include "esos_pic24_spi.h"

// LED Config
#define LED1 (_LATF4)
#define LED2 (_LATB14)
#define LED3 (_LATB15)

#define CONFIG_LED1() CONFIG_RF4_AS_DIG_OUTPUT()
#define CONFIG_LED2() CONFIG_RB14_AS_DIG_OUTPUT()
#define CONFIG_LED3() CONFIG_RB15_AS_DIG_OUTPUT()

#define TURN_ON_LED1() (LED1 = 1)
#define TURN_OFF_LED1() (LED1 = 0)
#define TURN_ON_LED2() (LED2 = 1)
#define TURN_OFF_LED2() (LED2 = 0)
#define TURN_ON_LED3() (LED3 = 0)
#define TURN_OFF_LED3() (LED3 = 1)

#define TOGGLE_LED3() (LED3 = !LED3)
#define TOGGLE_LED2() (LED2 = !LED2)
#define TOGGLE_LED1() (LED1 = !LED1)

// Switch config

inline void configSW3(void);

#define SW3 (_RC15)
#define SW3_IS_DEPRESSED() (readSW3WithDebounce() == 0)
#define SW3_IS_NOT_DEPRESSED() (!SW3_IS_DEPRESSED())

inline BOOL readSW3WithDebounce(void);

inline void configSW1(void);

#define SW1 (_RB13)
#define SW1_IS_DEPRESSED() (readSW1WithDebounce() == 0)
#define SW1_IS_NOT_DEPRESSED() (!SW1_IS_DEPRESSED())

inline BOOL readSW1WithDebounce(void);

inline void configSW2(void);

#define SW2 (_RB12)
#define SW2_IS_DEPRESSED() (readSW2WithDebounce() == 0)
#define SW2_IS_NOT_DPRESSED() (!SW2_IS_DEPRESSED)

inline BOOL readSW2WithDebounce(void);

// RPG config

inline void configRPGA(void);

#define RPGA (_RB8)
#define RPGA_IS_HIGH() (readRPGAWithDebounce() == 1)


inline BOOL readRPGAWithDebounce(void);

inline void configRPGB(void);

#define RPGB (_RB9)
#define RPGB_IS_HIGH() (readRPGBWithDebounce() == 1)

inline BOOL readRPGBWithDebounce(void);

// timing
#define MILLISECONDS_TO_TICKS(t) (t)
#define SECONDS_TO_TICKS(t) (MILLISECONDS_TO_TICKS(t * 1000))

// ADC
#define POT_CHANNEL (ESOS_SENSOR_CH02)
#define TEMP_SENSOR_CHANNEL (ESOS_SENSOR_CH03)
#define VREF (ESOS_SENSOR_VREF_3V0)

/*When you get the voltage from the sensor library it comes back in .1 mV
 *So to make things easier on ourselves these are already in .1mV
 */
#define TEMP_SENSOR_TENTH_MV_PER_DEGREES_C (63) // really 62.5 0.1mV but we'll round up
#define TEMP_SENSOR_TENTH_MV_OFFSET (4240)

// SPI
#define DAC_CS (_LATD0)
#define ENABLE_DAC_CS() (DAC_CS = 0)
#define DISABLE_DAC_CS() (DAC_CS = 1)
#define CONFIG_DAC_CS() CONFIG_RD0_AS_DIG_OUTPUT()

#define DAC_CONFIG_BITS (0x3000)
#define MAKE_DAC_PACKET(u16_d) ((u16_d & 0x0FFF) | DAC_CONFIG_BITS)
#define MAKE_DAC_PACKET_B(u16_d) (MAKE_DAC_PACKET(u16_d) | 0x8000)
#define WRITE_DAC(u16_data) do { \
		ENABLE_DAC_CS(); \
		ESOS_TASK_WAIT_ON_WRITE1SPI1(MAKE_DAC_PACKET(u16_data)); \
		DISABLE_DAC_CS(); \
	} while(0)

void configDAC(void);
void writeDac(uint16_t);
void writeDac_B(uint16_t);
#endif // REVF16_H_
