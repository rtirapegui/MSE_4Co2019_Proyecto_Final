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
 *  \file       uart0.c
 *  \brief      UART Driver of SERCOM 0 for PlatC
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.07.16  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "uart0.h"
#include "delay.h"
#include "platCbsp.h"
/* ----------------------------- Local macros ------------------------------ */
/* Converts a given SERCOM index to its interrupt vector index */
#define _SERCOM_INTERRUPT_VECT_NUM(n, unused)	SYSTEM_INTERRUPT_MODULE_SERCOM##n,

/* ------------------------------- Constants ------------------------------- */
#if defined(UART0_RX_INTERRUPT_DRIVEN_MODE) || defined(UART0_TX_INTERRUPT_DRIVEN_MODE)
#	include <ring_buffer.h>
#endif

/* ---------------------------- Local variables ---------------------------- */
static rbool_t             g_initFlag;
static struct usart_module g_UARTHandler;

#ifdef UART0_RX_INTERRUPT_DRIVEN_MODE
	static rui8_t				g_rxBuff[UART0_RX_BUFFER_SIZE];
	static struct ring_buffer	g_rxBuffHandler;
#endif

#ifdef UART0_TX_INTERRUPT_DRIVEN_MODE
	static rui8_t			   g_txBuff[UART0_TX_BUFFER_SIZE];
	static struct ring_buffer  g_txBuffHandler;
#endif

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
#if defined(UART0_RX_INTERRUPT_DRIVEN_MODE) || defined(UART0_TX_INTERRUPT_DRIVEN_MODE)
	void SERCOM0_Handler(void)
	{
		rui16_t interrupt_status;
		rui8_t error_code;

		/* Pointer to the hardware module instance */
		SercomUsart *const usart_hw = &(g_UARTHandler.hw->USART);

		if(!g_initFlag)
		{
			usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_MASK;		// Disable all interrupts
			return;
		}
	
		/* Wait for the synchronization to complete */
		_usart_wait_for_sync(&g_UARTHandler);

		/* Read and mask interrupt flag register */
		interrupt_status = usart_hw->INTFLAG.reg;
		interrupt_status &= usart_hw->INTENSET.reg;

		#ifdef UART0_TX_INTERRUPT_DRIVEN_MODE
			/* Check if a DATA READY interrupt has occurred, and if there is more to transfer */
			if (interrupt_status & SERCOM_USART_INTFLAG_DRE)
			{
				if(ring_buffer_is_empty(&g_txBuffHandler))
				{
					/* Disable the Data Register Empty Interrupt */
					usart_hw->INTENCLR.reg = SERCOM_USART_INTFLAG_DRE;
				}
				else
				{
					rui16_t data_to_send = (ring_buffer_get(&g_txBuffHandler) & 0x00FF);
			
					/* Write the data to send */
					usart_hw->DATA.reg = (data_to_send & SERCOM_USART_DATA_MASK);
				}
			}
		#endif

		#ifdef UART0_RX_INTERRUPT_DRIVEN_MODE
			/* Check if the Receive Complete interrupt has occurred, and that there's more data to receive */
			if (interrupt_status & SERCOM_USART_INTFLAG_RXC)
			{
				/* Read out the status code and mask away all but the 4 LSBs*/
				error_code = (rui8_t)(usart_hw->STATUS.reg & SERCOM_USART_STATUS_MASK);
		
				if(!error_code)
				{
					/* Read current packet from DATA register and save data into circular queue */
					rui16_t received_data = (usart_hw->DATA.reg & SERCOM_USART_DATA_MASK);

					if(!ring_buffer_is_full(&g_rxBuffHandler))
						ring_buffer_put(&g_rxBuffHandler, (rui8_t) received_data);
				}
				else
				{
					if (error_code & SERCOM_USART_STATUS_FERR)
					{
						usart_hw->STATUS.reg = SERCOM_USART_STATUS_FERR;		/* Clear flag by writing 1 to it */
					}
					else if (error_code & SERCOM_USART_STATUS_BUFOVF)
					{
						usart_hw->STATUS.reg = SERCOM_USART_STATUS_BUFOVF;		/* Clear flag by writing 1 to it */
					}
					else if (error_code & SERCOM_USART_STATUS_PERR)
					{
						usart_hw->STATUS.reg = SERCOM_USART_STATUS_PERR;		/* Clear flag by writing 1 to it */
					}
				}
			}
		#endif
	}
	static void _usart_interrupt_enable(rbool_t enable)
	{
		const rui8_t sercom_int_vectors[SERCOM_INST_NUM] =
		{
			MREPEAT(SERCOM_INST_NUM, _SERCOM_INTERRUPT_VECT_NUM, ~)
		};

		if(enable)
		{
			/* Enable Global interrupt for module */
			system_interrupt_enable(sercom_int_vectors[_sercom_get_sercom_inst_index(g_UARTHandler.hw)]);
		
			#ifdef UART0_RX_INTERRUPT_DRIVEN_MODE
				/* Enable the RX Complete Interrupt */
				g_UARTHandler.hw->USART.INTENSET.reg = SERCOM_USART_INTFLAG_RXC;
			#endif
		
		}
		else
		{
			/* Enable Global interrupt for module */
			system_interrupt_disable(sercom_int_vectors[_sercom_get_sercom_inst_index(g_UARTHandler.hw)]);
		
			#ifdef UART0_RX_INTERRUPT_DRIVEN_MODE
				/* Disable the RX Complete Interrupt */
				g_UARTHandler.hw->USART.INTENCLR.reg = SERCOM_USART_INTFLAG_RXC;
			#endif
		
			#ifdef UART0_TX_INTERRUPT_DRIVEN_MODE
				/* Disable the TX Complete Interrupt */
				g_UARTHandler.hw->USART.INTENCLR.reg = SERCOM_USART_INTFLAG_DRE;
			#endif
		}
	}
#endif
/* ---------------------------- Global functions --------------------------- */
rbool_t uart0_init(void)
{
	struct usart_config config_usart;

	if(g_initFlag)				// If already initialized (maybe by other module) return false
		return false;

	usart_get_config_defaults(&config_usart);

	config_usart.generator_source	= USART0_GENERATOR_SOURCE;
	config_usart.baudrate			= USART0_BAUDRATE;
	config_usart.data_order			= USART0_DATA_ORDER;
	config_usart.transfer_mode		= USART0_TRANSFER_MODE;
	config_usart.parity				= USART0_PARITY;
	config_usart.stopbits			= USART0_STOPBITS;
	config_usart.character_size		= USART0_CHARACTER_SIZE;
	
	config_usart.mux_setting		= USART0_MUX_SETTING;
	config_usart.pinmux_pad0		= USART0_PINMUX_PAD0;
	config_usart.pinmux_pad1		= USART0_PINMUX_PAD1;
	config_usart.pinmux_pad2		= USART0_PINMUX_PAD2;
	config_usart.pinmux_pad3		= USART0_PINMUX_PAD3;

	if(usart_init(&g_UARTHandler, SERCOM0, &config_usart) != STATUS_OK)
		return false;

	usart_enable(&g_UARTHandler);

#if defined(UART0_RX_INTERRUPT_DRIVEN_MODE) || defined(UART0_TX_INTERRUPT_DRIVEN_MODE)
	#ifdef UART0_RX_INTERRUPT_DRIVEN_MODE
		g_rxBuffHandler = ring_buffer_init(g_rxBuff, sizeof(g_rxBuff));
	#endif

	#ifdef UART0_TX_INTERRUPT_DRIVEN_MODE
		g_txBuffHandler = ring_buffer_init(g_txBuff, sizeof(g_txBuff));
	#endif

	_usart_interrupt_enable(true);
#endif
	
	g_initFlag = true;
	
	return true;
}
rbool_t uart0_enable(void)
{
	if(!g_initFlag)
		return false;
	
	usart_enable(&g_UARTHandler);

#if defined(UART0_RX_INTERRUPT_DRIVEN_MODE) || defined(UART0_TX_INTERRUPT_DRIVEN_MODE)
	_usart_interrupt_enable(true);
#endif
	
	return true;
}
rbool_t uart0_disable(void)
{
	if(!g_initFlag)
		return false;
	
	usart_disable(&g_UARTHandler);

#if defined(UART0_RX_INTERRUPT_DRIVEN_MODE) || defined(UART0_TX_INTERRUPT_DRIVEN_MODE)
	_usart_interrupt_enable(false);
#endif
	
	return true;
}
rbool_t uart0_readByteNoWait(rui8_t *byte)
{
	if(!g_initFlag || !byte)
		return false;
	
#ifdef UART0_RX_INTERRUPT_DRIVEN_MODE

	if(ring_buffer_is_empty(&g_rxBuffHandler))
		return false;

	*byte = ring_buffer_get(&g_rxBuffHandler);
	
#endif

#ifdef UART0_RX_POLLED_MODE
	
	rui16_t rx_data;
	
	if(STATUS_OK != usart_read_wait(&g_UARTHandler,	&rx_data))
		return false;
	
	*byte = (rui8_t) rx_data;
	
#endif

	return true;
}
rui8_t uart0_readData(rui8_t *buff, const rui8_t expBytesLen, const rui32_t timeout)
{
	rui32_t remainingBytes;
	rui32_t remainingLoops;
	
	if(!g_initFlag || !buff || !expBytesLen || !timeout)
		return false;
	
	remainingBytes = expBytesLen;
	remainingLoops = timeout;
	
	do
	{
		if(!uart0_readByteNoWait(buff))
		{
			delay_ms(1);
			
			if(remainingLoops)
				--remainingLoops;

			continue;
		}

		--remainingBytes;
		buff++;
	} while(remainingLoops && remainingBytes);
	
	return expBytesLen - remainingBytes;
}
rbool_t uart0_writeByte(const rui8_t byte)
{
	if(!g_initFlag)
		return false;
	
#ifdef UART0_TX_INTERRUPT_DRIVEN_MODE

	while(ring_buffer_is_full(&g_txBuffHandler))
		;
	
	ring_buffer_put(&g_txBuffHandler, byte);
	

	/* Enable the TX transmit holding register empty interrupt */
	g_UARTHandler.hw->USART.INTENSET.reg = SERCOM_USART_INTFLAG_DRE;
#endif

#ifdef UART0_TX_POLLED_MODE

	rui16_t tx_data = (byte & 0x00FF);
	
	if(STATUS_OK != usart_write_wait(&g_UARTHandler, tx_data))
		return false;

#endif
	
	return true;
}
rbool_t uart0_writeData(const rui8_t *buff, const rui32_t len)
{
	rui32_t i;
	rui16_t data_to_send;
	
	if(!len || !buff)
		return false;

	for(i = 0;i < len;i++)
	{
		data_to_send = (buff[i] & 0x00FF);
		usart_write_wait(&g_UARTHandler, data_to_send);
	}
	
	return true;
}
/* ------------------------------ End of file ------------------------------ */
