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
 *  \file       bsp.c
 *  \brief      Board Support Package for PlatC.
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.05.11  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  francuccilea@gmail.com
 *  RoTi  Rodrigo Tirapegui  rtirapeg@itba.edu.ar
 */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "bsp.h"
#include "system.h"
#include "delay.h"
#include "common_hw_timer.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
RKH_MODULE_NAME(bspModule)
#define TIME_SEC_COUNTER_RELOAD	RKH_TIME_MS(1000)

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
static uint32_t volatile g_tickCounter = TIME_SEC_COUNTER_RELOAD;
static uint32_t volatile g_timeSecCounter = 0;

/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
static void signalInitStart(void)
{
	LED_On(LED_RED_PIN);
	LED_On(LED_GREEN_PIN);
	LED_On(LED_BLUE_PIN);
}
static void signalInitEnd(void)
{
	LED_Off(LED_RED_PIN);
	LED_Off(LED_GREEN_PIN);
	LED_Off(LED_BLUE_PIN);
}
static void hwTimerExpiryCb(void)
{
	/* Invoke rkh tick */
	RKH_TIM_TICK(0);

	/* Increment time seconds counter every second */
	if(g_tickCounter && --g_tickCounter == 0)
	{
		g_tickCounter = TIME_SEC_COUNTER_RELOAD;
		++g_timeSecCounter;
	}

	common_tc_delay(RKH_TICK_RATE_MS * 1000);
}
/* -------------------------- External functions -------------------------- */
extern int errno;
extern int _end;
extern caddr_t _sbrk(int incr)
{
	static unsigned char *heap = NULL;
	unsigned char *prev_heap;

	if (heap == NULL) {
		heap = (unsigned char *)&_end;
	}
	prev_heap = heap;

	heap += incr;

	return (caddr_t) prev_heap;
}
/* ---------------------------- Global functions --------------------------- */
void bsp_init(void)
{
	/* Initialize system module */
	system_init();

	/* Initialize delay module */
	delay_init();

	/* Configure board pins */
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	pin_conf.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(PUSH_BUTTON_PIN, &pin_conf);
	port_pin_set_config(BATTERY_STATUS_PIN, &pin_conf);
	port_pin_set_config(BLE_STATUS_PIN, &pin_conf);
	port_pin_set_config(EXT_WAKE_UP_PIN, &pin_conf);
	
	pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_RED_PIN, &pin_conf);
	port_pin_set_config(LED_GREEN_PIN, &pin_conf);
	port_pin_set_config(LED_BLUE_PIN, &pin_conf);
	port_pin_set_config(BUZZER_PIN, &pin_conf);
	port_pin_set_config(BLE_RESET_PIN, &pin_conf);

	/* Set initial pin out status */
	BLE_RESET_SET();

	/* Fire initialization signaling start */
	signalInitStart();

	/* Initialize emulated eeprom */
	if(!emu_eeprom_init())
		RKH_ERROR();

	/* Initialize MPU9250 I2C hardware driver */
	if(!mpu9250_init())
		RKH_ERROR();

	/* Initialize push button */
	if(!push_button_init())
		RKH_ERROR();

	/* Initialize battery checker */
	if(!battery_init())
		RKH_ERROR();

	/* Initialize buzzer */
	if(!buzzer_init())
		RKH_ERROR();

	/* Initialize ble module */
	if(!bgm113_init())
		RKH_ERROR();

	/* Fire initialization signaling end */
	signalInitEnd();

	/* Initialize system tick counter */
	set_common_tc_expiry_callback(hwTimerExpiryCb);
	common_tc_init();
	common_tc_delay(RKH_TICK_RATE_MS * 1000);

	/* Initialize RKH state machine scheduler */
	rkh_fwk_init();

	/* Suppress trace events for all groups */
	RKH_FILTER_ON_GROUP(RKH_TRC_ALL_GROUPS);

	/* Suppress all trace event */
	RKH_FILTER_ON_EVENT(RKH_TRC_ALL_EVENTS);

	/* Enable all events into one specific group */
	RKH_FILTER_OFF_GROUP_ALL_EVENTS(RKH_TG_SM);

	/* Initialize trace stream */
	RKH_TRC_OPEN();

	/* Enable global interrupts */
	RKH_ENA_INTERRUPT();

	/* Set sleep mode to STANDBY */
	//system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
}
rui32_t bsp_getTimeSec(void)
{
	return g_timeSecCounter;
}
/* ------------------------------ End of file ------------------------------ */
