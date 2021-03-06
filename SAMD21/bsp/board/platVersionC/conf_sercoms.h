/**
 * \file
 *
 * \brief SAM D21 Sercoms configuration
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef CONF_SERCOMS_H_INCLUDED
#define CONF_SERCOMS_H_INCLUDED

/* SERCOM 2 - I2C */

// I2C baudrate
#define MPU9250_I2C_RATE				I2C_MASTER_BAUD_RATE_100KHZ // 400 kHz

/* SERCOM 0 - USART */

// USART #0 interface definitions
#define USART0_GENERATOR_SOURCE			GCLK_GENERATOR_0
#define USART0_BAUDRATE					115200
#define USART0_DATA_ORDER				USART_DATAORDER_LSB
#define USART0_TRANSFER_MODE			USART_TRANSFER_ASYNCHRONOUSLY
#define USART0_PARITY					USART_PARITY_NONE
#define USART0_STOPBITS					USART_STOPBITS_1
#define USART0_CHARACTER_SIZE			USART_CHARACTER_SIZE_8BIT

#define UART0_RX_BUFFER_SIZE			128		// Interrupt mode driven

#endif /* CONF_SERCOMS_H_INCLUDED */
