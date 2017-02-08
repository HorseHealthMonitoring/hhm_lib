#include "esos_comm_helpers.h"

ESOS_CHILD_TASK( __esos_OutUint16AsHexString, uint16_t u16_x) {
  static uint8_t        au8_String[6];
  static uint8_t        u8_c;
  static uint16_t       u16_tmp;

  ESOS_TASK_BEGIN();
  au8_String[0] = '0';
  au8_String[1] = 'x';
  u8_c = (u16_x >> 8);
  au8_String[2] = __esos_u8_GetMSBHexCharFromUint8(u8_c);
  au8_String[3] = __esos_u8_GetLSBHexCharFromUint8(u8_c);
  u8_c = (uint8_t)u16_x;
  au8_String[4] = __esos_u8_GetMSBHexCharFromUint8(u8_c);
  au8_String[5] = __esos_u8_GetLSBHexCharFromUint8(u8_c);
  u8_c = 0;

  while (u8_c < 6) {
    __ESOS_COMM_TXFIFO_PREP();
    ESOS_TASK_WAIT_WHILE(  u16_tmp == __st_TxBuffer.u16_Tail );
    __ESOS_COMM_WRITE_TXFIFO( au8_String[u8_c++] ); //write to buffer
    __esos_hw_signal_start_tx();
  } //end while()
  ESOS_TASK_END();

} // end __esos_OutUint16AsHexString()

ESOS_CHILD_TASK( __esos_OutPointOneMvAsMv, uint16_t u16_x) {
  // code provided by Gary Weasel
  static uint8_t      au8_String[7];
  static uint8_t      u8_c;
  static uint8_t      u8_cIndex;
  static uint8_t      u8_digit;
  static uint16_t     u16_tmp;

  ESOS_TASK_BEGIN();
  u8_cIndex = 0;
  u8_digit = 0;
  if (u16_x > 9999)
    au8_String[u8_digit++] = '0' + (u16_x / 10000);
  if (u16_x > 999)
    au8_String[u8_digit++] = '0' + ((u16_x % 10000) / 1000);
  if (u16_x > 99)
    au8_String[u8_digit++] = '0' + ((u16_x % 1000) / 100);
  au8_String[u8_digit++] = '0' + ((u16_x % 100) / 10);
  au8_String[u8_digit++] = '.';
  au8_String[u8_digit++] = '0' + (u16_x % 10);
  au8_String[u8_digit++] = 0;
  u8_c = au8_String[u8_cIndex++];
  while (u8_c != 0) {
    __ESOS_COMM_TXFIFO_PREP();
    ESOS_TASK_WAIT_WHILE( u16_tmp == __st_TxBuffer.u16_Tail );
    __ESOS_COMM_WRITE_TXFIFO( u8_c ); //write to buffer
    __esos_hw_signal_start_tx();
    u8_c = au8_String[u8_cIndex++];
  } //end while()
  ESOS_TASK_END();
} // end __esos_OutPointOneMvAsMv

ESOS_CHILD_TASK( __esos_OutSensorProcessType, esos_sensor_process_t e_processType) {
  static const char* psz_processTypeOut = "UNKNOWN: ";
  static uint8_t        u8_c;
  static uint8_t        u8_cIndex;
  static uint16_t       u16_tmp;

  ESOS_TASK_BEGIN();
  u8_cIndex = 0;
  switch (e_processType)
  {
      case ESOS_SENSOR_ONE_SHOT: psz_processTypeOut = "ONE SHOT: "; break;
      case ESOS_SENSOR_AVG2: psz_processTypeOut = "AVG 2: "; break;
      case ESOS_SENSOR_AVG4: psz_processTypeOut = "AVG 4: "; break;
      case ESOS_SENSOR_AVG8: psz_processTypeOut = "AVG 8: "; break;
      case ESOS_SENSOR_AVG16: psz_processTypeOut = "AVG 16: "; break;
      case ESOS_SENSOR_AVG32: psz_processTypeOut = "AVG 32: "; break;
      case ESOS_SENSOR_AVG64: psz_processTypeOut = "AVG 64: "; break;
      case ESOS_SENSOR_MIN2: psz_processTypeOut = "MIN 2: "; break;
      case ESOS_SENSOR_MIN4: psz_processTypeOut = "MIN 4: "; break;
      case ESOS_SENSOR_MIN8: psz_processTypeOut = "MIN 8: "; break;
      case ESOS_SENSOR_MIN16: psz_processTypeOut = "MIN 16: "; break;
      case ESOS_SENSOR_MIN32: psz_processTypeOut = "MIN 32: "; break;
      case ESOS_SENSOR_MIN64: psz_processTypeOut = "MIN 64: "; break;
      case ESOS_SENSOR_MAX2: psz_processTypeOut = "MAX 2: "; break;
      case ESOS_SENSOR_MAX4: psz_processTypeOut = "MAX 4: "; break;
      case ESOS_SENSOR_MAX8: psz_processTypeOut = "MAX 8: "; break;
      case ESOS_SENSOR_MAX16: psz_processTypeOut = "MAX 16: "; break;
      case ESOS_SENSOR_MAX32: psz_processTypeOut = "MAX 32: "; break;
      case ESOS_SENSOR_MAX64: psz_processTypeOut = "MAX 64: "; break;
      case ESOS_SENSOR_MEDIAN2: psz_processTypeOut = "MEDIAN 2: "; break;
      case ESOS_SENSOR_MEDIAN4: psz_processTypeOut = "MEDIAN 4: "; break;
      case ESOS_SENSOR_MEDIAN8: psz_processTypeOut = "MEDIAN 8: "; break;
      case ESOS_SENSOR_MEDIAN16: psz_processTypeOut = "MEDIAN 16: "; break;
      case ESOS_SENSOR_MEDIAN32: psz_processTypeOut = "MEDIAN 32: "; break;
      case ESOS_SENSOR_MEDIAN64: psz_processTypeOut = "MEDIAN 64: "; break;
      default: psz_processTypeOut = "UNKNOWN: "; break;
  }
  u8_c = psz_processTypeOut[u8_cIndex++];
  while (u8_c != 0) {
    __ESOS_COMM_TXFIFO_PREP();
    ESOS_TASK_WAIT_WHILE(  u16_tmp == __st_TxBuffer.u16_Tail );
    __ESOS_COMM_WRITE_TXFIFO( u8_c ); //write to buffer
    __esos_hw_signal_start_tx();
    u8_c = psz_processTypeOut[u8_cIndex++];
  } //end while()
  ESOS_TASK_END();

} // end __esos_OutUint16AsHexString()
