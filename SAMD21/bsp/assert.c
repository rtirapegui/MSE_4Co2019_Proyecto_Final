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
 *  \file       assert.c
 *  \brief      RKH assert function.
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.07.08  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  francuccilea@gmail.com
 *  RoTi  Rodrigo Tirapegui  rtirapeg@itba.edu.ar
 */
/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "reset.h"
#include "delay.h"
#include "platCbsp.h"

RKH_THIS_MODULE

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define ASSERT_DELAY		500		// In ms
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void rkh_assert(RKHROM char * const file, int line)
{
	(void)line;
	(void)file;

	RKH_DIS_INTERRUPT();
	RKH_TR_FWK_ASSERT( (RKHROM char *)file, __LINE__ );
	rkh_fwk_exit();

	/* Toggle leds every ASSERT_DELAY to indicate failure */
	LED_On(LED_RED_PIN);
	LED_On(LED_GREEN_PIN);
	LED_On(LED_BLUE_PIN);
	
	while(1)
	{
		LED_Toggle(LED_RED_PIN);
		LED_Toggle(LED_GREEN_PIN);
		LED_Toggle(LED_BLUE_PIN);
		delay_ms(ASSERT_DELAY);
	}
}
/* ------------------------------ End of file ------------------------------ */
