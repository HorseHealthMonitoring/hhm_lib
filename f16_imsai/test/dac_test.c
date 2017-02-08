#include "revF16.h"
#include "esos.h"
#include "esos_pic24_spi.h"
#include "esos_f14ui.h"
#include "esos_comm_helpers.h"

void writeDac(uint16_t u16_data)
{
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
    while( SPI1STAT & SPI_TX_BUFFER_FULL )
    {

    }
    // wait for RX word to be copied from SPI1SR
    while( !SPI1STAT || !SPI_RX_BUFFER_FULL )
    {

    }
    // read the word from SPI (clears SPI_RX_BUFFER_FULL bit)
    u16_scratch = SPI1BUF;

	DISABLE_DAC_CS();
}

ESOS_USER_TASK(dac_test)
{
	static uint16_t u16_data = 0xFFFF;
	ESOS_TASK_BEGIN();
	uint8_t u8_i = 0;
	while(TRUE)
	{
		

		esos_uiF14_toggleLED1();
		ESOS_TASK_WAIT_TICKS(5000);		
		
		u16_data = ~u16_data;
	}
	ESOS_TASK_END();
}

void user_init(void) 
{
	config_esos_uiF14();
	esos_uiF14_flashLED3(500);
	configDAC();
	esos_RegisterTask(dac_test);
}