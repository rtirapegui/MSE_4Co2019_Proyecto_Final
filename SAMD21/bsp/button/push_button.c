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
 *  \file       push_button.c
 *  \brief      Push Button Support Driver Header for PlatC
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.30  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */
 /* --------------------------------- Notes --------------------------------- */
 /* ----------------------------- Include files ----------------------------- */
 #include "push_button.h"
 #include "extint.h"
 #include "extint_callback.h"
 #include "platCbsp.h"

 /* ----------------------------- Local macros ------------------------------ */
 /* ------------------------------- Constants ------------------------------- */
 /* ---------------------------- Local variables ---------------------------- */
 
 /* User IRQ callback */
 static push_button_IRQCb g_userCb;

 /* ---------------------------- Local data types --------------------------- */
 /* ---------------------------- Global variables --------------------------- */
 /* ---------------------------- Local variables ---------------------------- */
 /* ----------------------- Local function prototypes ----------------------- */
 /* ---------------------------- Local functions ---------------------------- */
 static void PUSH_BUTTON_IRQCb(void)
 {
	if(g_userCb)
	{
		/* Invoke user callback */
		(g_userCb) (PUSH_BUTTON_ACTIVE == port_pin_get_input_level(PUSH_BUTTON_PIN));
	}
 }
 /* ---------------------------- Global functions --------------------------- */
 rbool_t push_button_init(void)
 {
	struct extint_chan_conf extint_config;

	 /* Configure external interrupt */
	 extint_chan_get_config_defaults(&extint_config);

	 extint_config.gpio_pin            = PUSH_BUTTON_IRQ_PIN;
	 extint_config.gpio_pin_mux        = PUSH_BUTTON_IRQ_MUX;
	 extint_config.gpio_pin_pull       = EXTINT_PULL_NONE;
	 extint_config.detection_criteria  = EXTINT_DETECT_BOTH;
	 extint_config.filter_input_signal = true;

	 extint_chan_set_config(PUSH_BUTTON_IRQ_LINE, &extint_config);

	 /* Configure external interrupt callback */
	 extint_register_callback(PUSH_BUTTON_IRQCb, PUSH_BUTTON_IRQ_LINE, EXTINT_CALLBACK_TYPE_DETECT);

	 /* Enable external interrupt callback */
	 extint_chan_enable_callback(PUSH_BUTTON_IRQ_LINE, EXTINT_CALLBACK_TYPE_DETECT);

	 return true;
 }
 void push_button_registerCb(push_button_IRQCb cb)
 {
	 if(cb)
	 {
		 RKH_ENTER_CRITICAL();
		 g_userCb = cb;
		 RKH_EXIT_CRITICAL();
	 }
 }
 /* ------------------------------ End of file ------------------------------ */

