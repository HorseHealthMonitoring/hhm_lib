#include "revF16.h"

inline void configSW1(void)
{
  CONFIG_RB13_AS_DIG_INPUT();
  ENABLE_RB13_PULLUP();
  DELAY_US(1);
}

inline void configSW2(void)
{
  CONFIG_RB12_AS_DIG_INPUT();
  ENABLE_RB12_PULLUP();
  DELAY_US(1);
}

inline void configSW3(void)
{
  CONFIG_RC15_AS_DIG_INPUT();
  ENABLE_RC15_PULLUP();
  DELAY_US(1);
}

inline void configRPGA(void)
{
  CONFIG_RB8_AS_DIG_INPUT();
  ENABLE_RB8_PULLUP();
  DELAY_US(1);
}

inline void configRPGB(void)
{
  CONFIG_RB9_AS_DIG_INPUT();
  ENABLE_RB9_PULLUP();
  DELAY_US(1);
}

inline BOOL readSWWithDebounce(BOOL (*fn_readSwitch)(void));

inline BOOL readSW3(void)
{
  return SW3;
}

inline BOOL readSW3WithDebounce(void)
{
  return readSWWithDebounce(readSW3);
}

inline BOOL readSW2(void)
{
  return SW2;
}

inline BOOL readSW2WithDebounce(void)
{
  return readSWWithDebounce(readSW2);
}

inline BOOL readSW1(void)
{
  return SW1;
}

inline BOOL readSW1WithDebounce(void)
{
  return readSWWithDebounce(readSW1);
}

inline BOOL readRPGA(void)
{
  return RPGA;
}

inline BOOL readRPGAWithDebounce(void)
{
  return readSWWithDebounce(readRPGA);
}

inline BOOL readRPGB(void)
{
  return RPGB;
}

inline BOOL readRPGBWithDebounce(void)
{
  return readSWWithDebounce(readRPGB);
}

// Reads the switch using the function provided and returns when
// the switch reads the same value twice.
inline BOOL readSWWithDebounce(BOOL (*fn_readSwitch)(void))
{
  BOOL oldSwValue;
  BOOL newSwValue;
  do
  {
    oldSwValue = fn_readSwitch();
    DELAY_US(10);
    newSwValue = fn_readSwitch();
  } while (oldSwValue != newSwValue);
  return oldSwValue;
}

void configSPI1(void) {
    SPI1CON1 =
    //spi clock = 60MHz/1*6 = 60MHz/4 = 10MHz
    PRI_PRESCAL_1_1 |     //1:1 primary prescale
    SEC_PRESCAL_2_1 |     //6:1 secondary prescale
    CLK_POL_ACTIVE_HIGH   | //clock active high (CKP = 0)
    SPI_CKE_ON          | //out changes active to inactive (CKE=1)
    SPI_MODE16_ON        | //8-bit mode
    MASTER_ENABLE_ON;     //master mode
    CONFIG_SDO1_TO_RP(RD4_RP);
    CONFIG_SCK1OUT_TO_RP(RD5_RP);

    SPI1STATbits.SPIEN = 1;  //enable SPI mode
}

void configDAC(void) {
    CONFIG_DAC_CS();
    DISABLE_DAC_CS(); // explicitely disable able to begin

    configSPI1();
}

static BOOL b_dacInUseFlag = FALSE;

void writeDac(uint16_t u16_data)
{
    while(b_dacInUseFlag);
    b_dacInUseFlag = TRUE;
    ENABLE_DAC_CS();
    // clear the overflow flag, just in case it is set
    if (SPI1STATbits.SPIROV) SPI1STATbits.SPIROV = 0;
    //clear SPI interrupt flag since we are about to write new value to SPI
    _SPI1IF = 0;
    /* read SPI1BUF to clear SPI_RX_BUFFER_FULL bit just in case previous
     SPI use did not read the SPI1BUF that last time!
    */
      uint16_t u16_scratch = SPI1BUF;
    SPI1BUF = MAKE_DAC_PACKET(u16_data);

    /* Seen some strange behavior checking _SPI1IF like the
     * hardware support library.  The following method is valid
     * and appears to work in all cases.
     */
    // wait for TX word to be copied to SPI1SR
    while( SPI1STAT & SPI_TX_BUFFER_FULL );
    // wait for RX word to be copied from SPI1SR
    while( !(SPI1STAT & SPI_RX_BUFFER_FULL) );
    // read the word from SPI (clears SPI_RX_BUFFER_FULL bit)
    u16_scratch = SPI1BUF;

    DISABLE_DAC_CS();
    b_dacInUseFlag = FALSE;
}

void writeDac_B(uint16_t u16_data)
{
    while(b_dacInUseFlag);
    b_dacInUseFlag = TRUE;
    ENABLE_DAC_CS();
    // clear the overflow flag, just in case it is set
    if (SPI1STATbits.SPIROV) SPI1STATbits.SPIROV = 0;
    //clear SPI interrupt flag since we are about to write new value to SPI
    _SPI1IF = 0;
    /* read SPI1BUF to clear SPI_RX_BUFFER_FULL bit just in case previous
     SPI use did not read the SPI1BUF that last time!
    */
      uint16_t u16_scratch = SPI1BUF;
    SPI1BUF = MAKE_DAC_PACKET_B(u16_data);

    /* Seen some strange behavior checking _SPI1IF like the
     * hardware support library.  The following method is valid
     * and appears to work in all cases.
     */
    // wait for TX word to be copied to SPI1SR
    while( SPI1STAT & SPI_TX_BUFFER_FULL );
    // wait for RX word to be copied from SPI1SR
    while( !(SPI1STAT & SPI_RX_BUFFER_FULL) );
    // read the word from SPI (clears SPI_RX_BUFFER_FULL bit)
    u16_scratch = SPI1BUF;

    DISABLE_DAC_CS();
    b_dacInUseFlag = FALSE;
}