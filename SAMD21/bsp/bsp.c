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
#include "rkh.h"
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
void hwTimerExpiryCb(void)
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

/* ---------------------------- Global functions --------------------------- */
void bsp_init(void)
{
	/* Initialize system module */
	system_init();

	/* Initialize delay module */
	delay_init();

	/* Initialize system tick counter */
	set_common_tc_expiry_callback(hwTimerExpiryCb);
	common_tc_init();
	common_tc_delay(RKH_TICK_RATE_MS * 1000);

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

	/* Initialize battery checker */
	if(!bgm113_init())
		RKH_ERROR();

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
}
rui32_t bsp_getTimeSec(void)
{
	return g_timeSecCounter;
}
rui8_t bsp_writeEepromData(emu_eeprom_id_t id, const rui8_t *data)
{
	return emu_eeprom_writeData(id, data);
}
rui8_t bsp_readEepromData(emu_eeprom_id_t id, const rui8_t *data)
{
	return emu_eeprom_readData(id, data);
}
rbool_t bsp_setMotionThreshold(rui16_t womThreshold)
{
	return mpu9250_setMotionThreshold(womThreshold);
}

/* ------------------------------ End of file ------------------------------ */
