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
 *  \file       platCbsp.h
 *  \brief      Board Support Package PlatC.
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.05.11  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */

/* --------------------------------- Module -------------------------------- */

#ifndef __PLATCBSP_H__
#define __PLATCBSP_H__

/* ----------------------------- Include files ----------------------------- */
#include "port.h"
#include "adc.h"
#include "usart.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
#ifdef BOARD_NAME
#undef BOARD_NAME
#define BOARD_NAME "PLAT-VERSION_C"
#endif

/* ================================== GPIO ================================= */

/* MPU9250 */    
#ifndef EXT_WAKE_UP_PIN
#define EXT_WAKE_UP_PIN				PIN_PA19
#endif
#ifndef EXT_WAKE_UP_PORT
#define EXT_WAKE_UP_PORT			PORT_PA19
#endif
#ifndef EXT_WAKE_UP_IRQ_PIN
#define EXT_WAKE_UP_IRQ_PIN			PIN_PA19A_EIC_EXTINT3
#endif
#ifndef EXT_WAKE_UP_IRQ_PORT
#define EXT_WAKE_UP_IRQ_PORT		PORT_PA19A_EIC_EXTINT3
#endif
#ifndef EXT_WAKE_UP_IRQ_MUX
#define EXT_WAKE_UP_IRQ_MUX			MUX_PA19A_EIC_EXTINT3
#endif
#ifndef EXT_WAKE_UP_IRQ_PINMUX
#define EXT_WAKE_UP_IRQ_PINMUX		PINMUX_PA19A_EIC_EXTINT3
#endif
#ifndef EXT_WAKE_UP_IRQ_LINE
#define EXT_WAKE_UP_IRQ_LINE		3
#endif
#define EXT_WAKE_UP_NAME			"EXT_WAKE_UP"

/* ================================== I2C ================================= */
/* Master sercom pinmux setting */
#define MPU9250_I2C_MASTER_MODULE	SERCOM2

/* Push button */
#define PUSH_BUTTON_ACTIVE			0U
#define PUSH_BUTTON_INACTIVE		!PUSH_BUTTON_ACTIVE

#ifndef PUSH_BUTTON_PIN
#define PUSH_BUTTON_PIN				PIN_PA10
#endif
#ifndef PUSH_BUTTON_PORT
#define PUSH_BUTTON_PORT			PORT_PA10
#endif
#ifndef PUSH_BUTTON_IRQ_PIN
#define PUSH_BUTTON_IRQ_PIN			PIN_PA10A_EIC_EXTINT10
#endif
#ifndef PUSH_BUTTON_IRQ_PORT
#define PUSH_BUTTON_IRQ_PORT		PORT_PA10A_EIC_EXTINT10
#endif
#ifndef PUSH_BUTTON_IRQ_MUX
#define PUSH_BUTTON_IRQ_MUX			MUX_PA10A_EIC_EXTINT10
#endif
#ifndef PUSH_BUTTON_IRQ_PINMUX
#define PUSH_BUTTON_IRQ_PINMUX		PINMUX_PA10A_EIC_EXTINT10
#endif
#ifndef PUSH_BUTTON_IRQ_LINE
#define PUSH_BUTTON_IRQ_LINE		10
#endif
#define PUSH_BUTTON_NAME			"PUSH_BUTTON"

#define PUSH_BUTTON_GET_STATUS()	(PUSH_BUTTON_ACTIVE == port_pin_get_input_level(PUSH_BUTTON_PIN))

/* Led RGB */
#define LED_ACTIVE					0U
#define LED_INACTIVE				!LED_ACTIVE

#ifndef LED_RED_PIN
#define LED_RED_PIN					PIN_PA16
#endif
#ifndef LED_GREEN_PIN
#define LED_GREEN_PIN				PIN_PA18
#endif
#ifndef LED_BLUE_PIN
#define LED_BLUE_PIN				PIN_PA17
#endif

/**
 * \brief Turns off the specified LEDs.
 *
 * \param led_pin LED to turn off (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_Off(led_pin)				port_pin_set_output_level(led_pin,LED_INACTIVE)

/**
 * \brief Turns on the specified LEDs.
 *
 * \param led_pin LED to turn on (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_On(led_pin)					port_pin_set_output_level(led_pin,LED_ACTIVE)

/**
 * \brief Toggles the specified LEDs.
 *
 * \param led_pin LED to toggle (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_Toggle(led_pin)				port_pin_toggle_output_level(led_pin)

/*	Buzzer	*/
#ifndef BUZZER_PIN
#define BUZZER_PIN						PIN_PA11
#endif

#define BUZZER_PWM_MODULE				TCC0
#define BUZZER_PWM_CHANNEL				3
#define BUZZER_PWM_OUTPUT				3
#define BUZZER_PWM_PIN					PIN_PA11F_TCC0_WO3
#define BUZZER_PWM_MUX					MUX_PA11F_TCC0_WO3
#define BUZZER_PWM_PINMUX				PINMUX_PA11F_TCC0_WO3

/*	Battery	*/
#define BATTERY_STATUS_CHARGING			0U
#define BATTERY_STATUS_NOT_CHARGING		!BATTERY_STATUS_CHARGING

#ifndef BATTERY_STATUS_PIN
#define BATTERY_STATUS_PIN				PIN_PA02
#endif
#ifndef BATTERY_STATUS_PORT
#define BATTERY_STATUS_PORT				PORT_PA02
#endif
#ifndef BATTERY_STATUS_IRQ_PIN
#define BATTERY_STATUS_IRQ_PIN			PIN_PA02A_EIC_EXTINT2
#endif
#ifndef BATTERY_STATUS_IRQ_PORT
#define BATTERY_STATUS_IRQ_PORT			PORT_PA02A_EIC_EXTINT2
#endif
#ifndef BATTERY_STATUS_IRQ_MUX
#define BATTERY_STATUS_IRQ_MUX			MUX_PA02A_EIC_EXTINT2
#endif
#ifndef BATTERY_STATUS_IRQ_PINMUX
#define BATTERY_STATUS_IRQ_PINMUX		PINMUX_PA02A_EIC_EXTINT2
#endif
#ifndef BATTERY_STATUS_IRQ_LINE
#define BATTERY_STATUS_IRQ_LINE			2
#endif
#ifndef BATTERY_STATUS_POSITIVE_INPUT
#define BATTERY_STATUS_POSITIVE_INPUT	ADC_POSITIVE_INPUT_PIN0
#endif

#define BATTERY_STATUS_GET_STATUS()		(BATTERY_STATUS_CHARGING == port_pin_get_input_level(BATTERY_STATUS_PIN))

/*	BGM 113	*/
#define BLE_RESET_ACTIVE				0U
#define BLE_RESET_INACTIVE				!BLE_RESET_ACTIVE

#ifndef BLE_RESET_PIN
#define BLE_RESET_PIN					PIN_PA27
#endif
#ifndef BLE_RESET_PORT
#define BLE_RESET_PORT					PORT_PA27
#endif

#define BLE_STATUS_CONNECTED			0U
#define BLE_STATUS_DISCONNECTED			!BLE_STATUS_CONNECTED

#ifndef BLE_STATUS_PIN
#define BLE_STATUS_PIN					PIN_PA28
#endif
#ifndef BLE_STATUS_PORT
#define BLE_STATUS_PORT					PORT_PA28
#endif
#ifndef BLE_STATUS_IRQ_PIN
#define BLE_STATUS_IRQ_PIN				PIN_PA28A_EIC_EXTINT8
#endif
#ifndef BLE_STATUS_IRQ_PORT
#define BLE_STATUS_IRQ_PORT				PORT_PA28A_EIC_EXTINT8
#endif
#ifndef BLE_STATUS_IRQ_MUX
#define BLE_STATUS_IRQ_MUX				MUX_PA28A_EIC_EXTINT8
#endif
#ifndef BLE_STATUS_IRQ_PINMUX
#define BLE_STATUS_IRQ_PINMUX			PINMUX_PA28A_EIC_EXTINT8
#endif
#ifndef BLE_STATUS_IRQ_LINE
#define BLE_STATUS_IRQ_LINE				8
#endif

#define BLE_STATUS_GET_STATUS()			(BLE_STATUS_CONNECTED == port_pin_get_input_level(BLE_STATUS_PIN))

/**
 * \brief Enable BLE module.
 *
 * \note The BLE_RESET pin is set to GPIO output mode.
 */
#define BLE_RESET_RELEASE()				port_pin_set_output_level(BLE_RESET_PIN, BLE_RESET_INACTIVE)

/**
 * \brief Disable BLE module.
 *
 * \note The BLE_RESET pin is set to GPIO output mode.
 */
#define BLE_RESET_SET()					port_pin_set_output_level(BLE_RESET_PIN, BLE_RESET_ACTIVE)
       
/* ================================== UART ================================= */
#define BLE_UART_MODULE					SERCOM0
#define USART0_MUX_SETTING				USART_RX_1_TX_0_XCK_1
#define USART0_PINMUX_PAD0				PINMUX_PA04D_SERCOM0_PAD0
#define USART0_PINMUX_PAD1				PINMUX_PA05D_SERCOM0_PAD1
#define USART0_PINMUX_PAD2				PINMUX_UNUSED
#define USART0_PINMUX_PAD3				PINMUX_UNUSED

#define BLE_UART_TX_PIN					PIN_PA04
#define BLE_UART_TX_PORT				PORT_PA04

#define BLE_UART_RX_PIN					PIN_PA05
#define BLE_UART_RX_PORT				PORT_PA05

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ File footer ------------------------------ */
