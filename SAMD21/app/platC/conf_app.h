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
 *  \file       conf_app.h
 *  \brief      Application default values
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.07.08  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */

 /* --------------------------------- Module -------------------------------- */
#ifndef CONF_APP_H_
#define CONF_APP_H_

/* ----------------------------- Include files ----------------------------- */

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define ADVERTISE_NAME_SIZE_MAX		20
#define MOTION_THRESHOLD_MAX		1020	
#define MOTION_THRESHOLD_MIN		40

#define ADVERTISE_NAME_DEFAULT		"PlatC"
#define MOTION_THRESHOLD_DEFAULT	952				// in mg
#define BUZZER_ENABLED_DEFAULT		true
#define MOTION_LED_PIN				LED_RED_PIN
#define BATTERY_LED_PIN				LED_GREEN_PIN	
#define BLE_LED_PIN					LED_BLUE_PIN

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
