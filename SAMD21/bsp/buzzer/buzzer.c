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
 *  \file       buzzer.c
 *  \brief      Buzzer
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
 #include "conf_clocks.h"

 #include "buzzer.h"
 #include "tcc.h"
 #include "platCbsp.h"
 /* ----------------------------- Local macros ------------------------------ */
 /* ------------------------------- Constants ------------------------------- */
 #define PWM_PERIOD_US	125	// In us
 /* ---------------------------- Local variables ---------------------------- */
 struct tcc_module g_tcc_instance;
 /* ---------------------------- Local data types --------------------------- */
 /* ---------------------------- Global variables --------------------------- */
 /* ---------------------------- Local variables ---------------------------- */
 /* ----------------------- Local function prototypes ----------------------- */
 /* ---------------------------- Local functions ---------------------------- */
 /* ---------------------------- Global functions --------------------------- */
 rbool_t buzzer_init(void)
 {
	struct tcc_config config_tcc;

	tcc_get_config_defaults(&config_tcc, BUZZER_PWM_MODULE);

	config_tcc.counter.clock_source	   = CONF_CLOCK_GCLK_4_ENABLE;
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV8;
	config_tcc.counter.period		   = PWM_PERIOD_US;
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_NORMAL_FREQ;

	config_tcc.pins.enable_wave_out_pin[BUZZER_PWM_OUTPUT] = true;
	config_tcc.pins.wave_out_pin[BUZZER_PWM_OUTPUT]        = BUZZER_PWM_PIN;
	config_tcc.pins.wave_out_pin_mux[BUZZER_PWM_OUTPUT]    = BUZZER_PWM_MUX;

	tcc_init(&g_tcc_instance, BUZZER_PWM_MODULE, &config_tcc);

	return true;
 }
 void buzzer_On(void)
 {
	tcc_enable(&g_tcc_instance);
 }
 void buzzer_Off(void)
 {
	tcc_disable(&g_tcc_instance);
 }
 /* ------------------------------ End of file ------------------------------ */

