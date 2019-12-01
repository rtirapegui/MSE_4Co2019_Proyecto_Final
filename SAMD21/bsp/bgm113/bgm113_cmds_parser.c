/*
 *  --------------------------------------------------------------------------
 *
 *                                    PlatC
 *                                -------------
 *
 *                  Coin-sized programmable multi-sensor node
 *
 *  PlatC is an open-source, programmable multi-sensor node,
 *  specifically designed for wearables but with endless opportunities.
 *
 *  PlatC is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any
 *  later version.
 *
 *  PlatC is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with RKH, see copying.txt file.
 *
 *  Contact information:
 *  PlatC web site: @todo add web site
 *  e-mail:         @todo add e-mail
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file       bgm113_cmds_parser.c
 *  \brief      BGM 113 Bluetooth Module Cmds Parser Driver for PlatC
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.07.27  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */
 /* --------------------------------- Notes --------------------------------- */
 
 /*
 * BLE target protocol:
 *
 * (1 byte)    (1 byte)	      (1 byte)		   (pl bytes)	  (1 byte)
 *
 *   STX	+	seqNo	+	payloadLength	+	payload		+	ETX
 *
 */

 /* ----------------------------- Include files ----------------------------- */
 #include <string.h>
 #include "bgm113_cmds_parser.h"
 #include "uart0.h"
 #include "platCbsp.h"
 /* ----------------------------- Local macros ------------------------------ */
 /* ------------------------------- Constants ------------------------------- */
 
 /* BLE target protocol control characters */
 #define STX 0x02
 #define ETX 0x03

 /**/
 #define CMD_MAX_FRAME_SIZE          128
 #define CMD_UART_RCV_DATA_TIMEOUT   100     // In milliseconds

 /* BLE Target protocol states */
 typedef enum
 {
	CMD_RCV_IDLE            = 0x00
 ,	CMD_RCV_WAITING_SEQ_NO
 ,	CMD_RCV_WAITING_EXP_LEN
 ,	CMD_RCV_WAITING_PAYLOAD_AND_ETX
 } cmdRcvState_t;
 
 /* ---------------------------- Local variables ---------------------------- */
  static rui8_t g_seqNo;
  static rui8_t g_dataSize;
  static rui8_t g_data[CMD_MAX_FRAME_SIZE];
 /* ---------------------------- Local data types --------------------------- */
 /* ---------------------------- Global variables --------------------------- */
 /* ---------------------------- Local variables ---------------------------- */
 /* ----------------------- Local function prototypes ----------------------- */
 /* ---------------------------- Local functions ---------------------------- */
 static rui8_t getSeqNo(void)
 {
	static rui8_t seqNo;

	if(!seqNo)
		seqNo++;

	return seqNo;
 }
 static rbool_t getCompleteMessage(rui32_t timeMs)
{
	static cmdRcvState_t	state = CMD_RCV_IDLE;
	static rui32_t          timeoutMs = 0;
	static rui8_t           _payldByteCnt;
	rui8_t                  byte;
	rbool_t                 keepGoing = true, ret = false;
  
	/* Reset state when timeout */
	if(timeoutMs && (timeMs >= timeoutMs))
	{
		state = CMD_RCV_IDLE;
		timeoutMs = 0;
	}
  
	/* 
	* Advance state machine at maximum speed until there's no more data
	* inside UART buffer.
	* Consider that mostly UART byte transfers are slower that CPU speed,
	* so maybe one full message isn't available at one time.
	* */
	while(keepGoing) 
	{
		/* Get byte from uart */
		if(true == (keepGoing = uart0_readByteNoWait(&byte))) 
		{
			switch(state) 
			{
				case CMD_RCV_IDLE:
				{
					if(STX == byte) 
					{
						/* Kick timeout */
						timeoutMs = timeMs + CMD_UART_RCV_DATA_TIMEOUT;
						state  = CMD_RCV_WAITING_SEQ_NO;
					}
				} break;
				case CMD_RCV_WAITING_SEQ_NO:
				{
					/* Kick timeout */
					timeoutMs = timeMs + CMD_UART_RCV_DATA_TIMEOUT;
					state  = CMD_RCV_WAITING_EXP_LEN;
          
					/* Save message cmdId */
					g_seqNo = byte;
				} break;
				case CMD_RCV_WAITING_EXP_LEN:
				{
					/* Kick timeout */
					timeoutMs = timeMs + CMD_UART_RCV_DATA_TIMEOUT;
					state = CMD_RCV_WAITING_PAYLOAD_AND_ETX;
            
					/* Save message payload's expected length */
					g_dataSize = byte;
            
					/* Reset payload data byte counter */
					_payldByteCnt = 0;

				} break;
				case CMD_RCV_WAITING_PAYLOAD_AND_ETX:
				{
					/* Kick timeout */
					timeoutMs = timeMs + CMD_UART_RCV_DATA_TIMEOUT;
          
					if(_payldByteCnt < g_dataSize) 
					{
						/* Save current payload byte in buffer */
						g_data[_payldByteCnt++] = byte;
					} 
					else 
					{
						/* Now should come the ETX byte */
						if(ETX == byte) 
						{
							/* Cancel receive data timeout timer and return message as 
							 * completed */
							timeoutMs = 0;
              
							/* Return to idle state */
							state = CMD_RCV_IDLE;

							/* Invoke ble_cmds_parser_processIncomingMsg */
							ret = true;
						}
            
						/* Get out from while loop */
						keepGoing = false;
					}
				} break;
        
				default:
				{
					state = CMD_RCV_IDLE;
				} break;
			}
		}
	}

  return ret;
}
 /* ---------------------------- Global functions --------------------------- */
 rbool_t bgm113_cmds_parser_init(void)
 {
	 /* Enable uart module */
	 if(!uart0_init())
		return false;

	 return true;
 }
 rbool_t bgm113_cmds_parser_sendData(const rui8_t *buff)
 {
	if(buff)
	{
		rui32_t len = strlen((const char *) buff);

		uart0_writeByte(STX);			/* Send STX				*/
		uart0_writeByte(getSeqNo());	/* Send seqNo			*/
		uart0_writeByte(len);			/* Send payload length	*/
		uart0_writeData(buff, len);		/* Send payload			*/
		uart0_writeByte(ETX);			/* Send ETX				*/
	}

	return false;
 }
 void bgm113_cmds_parser_processIncomingDataTask(rui32_t timeMs)
 {
	if(true == getCompleteMessage(timeMs))
		bgm113_cmds_parser_indDataHandler(g_seqNo, g_data, g_dataSize);
 }
  /* ------------------------------ End of file ------------------------------ */