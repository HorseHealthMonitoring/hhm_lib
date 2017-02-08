// .. "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
//    All rights reserved.
//    (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
//    (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
//    (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
//
//    Permission to use, copy, modify, and distribute this software and its
//    documentation for any purpose, without fee, and without written agreement is
//    hereby granted, provided that the above copyright notice, the following
//    two paragraphs and the authors appear in all copies of this software.
//
//    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
//    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
//    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
//    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
//    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
//    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
//    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
//
//    Please maintain this header in its entirety when copying/modifying
//    these files.
//
// *****************************
// app_ecan_receiver.c - receive a message using the CANFactory
// *****************************
// ESOS application program to receive a mail message using the CANFactory task.

// Includes
#include    <string.h>
#include    "esos.h"
#include    "esos_pic24.h"
#include    "esos_ecan_imsai.h"
#include    "esos_comm.h"

char psz_newLine[3] = {0x0D, 0x0A, 0};
uint8_t u8_activeFlag = 0;

ESOS_USER_TASK ( snifferMenu )
{
    static uint8_t u8_snifferMenuChoice;

    ESOS_TASK_BEGIN();
    while(TRUE)
    {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("Welcome to the ECAN Sniffer.");
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_WAIT_ON_SEND_STRING("Please select your mode of operation.");
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_WAIT_ON_SEND_STRING("(0) Sleeping.");
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_WAIT_ON_SEND_STRING("(1) Active.");
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        do
        {
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_snifferMenuChoice);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
        } while ((u8_snifferMenuChoice < '0') || (u8_snifferMenuChoice > '1'));

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        u8_activeFlag = (uint8_t)(u8_snifferMenuChoice - '0');
        if (u8_activeFlag == 0)
        {
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("Sleeping.");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            do
            {
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(u8_snifferMenuChoice);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
            } while (u8_snifferMenuChoice != '1');
        }
        else
        {
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("Active.");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            do
            {
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(u8_snifferMenuChoice);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
            } while (u8_snifferMenuChoice != '0');
        }
    }// end of while
    ESOS_TASK_END();
}

ESOS_USER_TASK( sniffer )
{
    static uint16_t u16_canID;
    static uint8_t u8_msgLength;
    static MAILMESSAGE msg;

    ESOS_TASK_BEGIN();
    esos_ecan_canfactory_subscribe(__pstSelf, 0xFFF, 0xFFFF, MASKCONTROL_FIELD_NONZERO);
    while (TRUE)
    {
        while (u8_activeFlag == 1)
        {
            ESOS_TASK_WAIT_FOR_MAIL();
            ESOS_TASK_GET_NEXT_MESSAGE(&msg);
            u16_canID = msg.au16_Contents[0];
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("ECAN ID: ");
            ESOS_TASK_WAIT_ON_SEND_UINT8(u16_canID);
            ESOS_TASK_WAIT_ON_SEND_UINT8(u16_canID >> 8);
            ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
            ESOS_TASK_WAIT_ON_SEND_STRING("MESSAGE DATA: ");
            //u8_msgLength = ESOS_GET_MSG_DATA_LENGTH(msg);
            uint8_t u8_i;
            for (u8_i = 2; u8_i < u8_msgLength; u8_i++)
            {
                ESOS_TASK_WAIT_ON_SEND_STRING(psz_newLine);
                ESOS_TASK_WAIT_ON_SEND_STRING("0x");
                ESOS_TASK_WAIT_ON_SEND_UINT8(msg.au8_Contents[u8_i]);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
    }
    ESOS_TASK_END();
}

/****************************************************
 *  user_init()
 ****************************************************
 */
void user_init ( void ) {
    __esos_unsafe_PutString( HELLO_MSG );
    __esos_ecan_hw_config_ecan();
    CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);
    esos_RegisterTask(CANFactory);
    esos_RegisterTask(snifferMenu);
    esos_RegisterTask(sniffer);
}
