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
 *  \file       platC.h
 *  \brief      PlatC SMA Header for PlatC Demo.
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
#ifndef _PLAT_C_H_
#define _PLAT_C_H_

/* ----------------------------- Include files ----------------------------- */

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* -------------------------------- Signals -------------------------------- */
typedef enum
{
	evInitTout,
	evCmdTout,
	evFailureToggle,
	evConnStatusTout,
	evUnlikedTout,
	evMotionIndicatorTout,
	evConnected,
	evDisconnected,
	evOk,
	evError,
	evStartAdvertising,
	evStopAdvertising,
	evPushbuttonShortPress,
	evPushbuttonLongPress,
	evForceDisconnect,
	evMotionDetect,
	evPlatCCfg,
	evSporaGetCfg,
	evPlatCGetData,
	evBatteryChargerConnected,
	evBatteryChargerDisconnected,

	PLATC_NUM_EVENTS
} platC_SMA_signals_t;

/* -------------------------------- Events --------------------------------- */

/* ------------------------- Declares Active Objects ----------------------- */
RKH_SMA_DCLR(platC);

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
