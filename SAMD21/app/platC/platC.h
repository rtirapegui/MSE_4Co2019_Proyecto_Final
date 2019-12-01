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
 *  \ingroup    app
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
#include "conf_app.h"

#include "bsp.h"

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
	evPlatCGetCfg,
	evPlatCGetData,
	evPlatCFitness,
	evBatteryChargerConnected,
	evBatteryChargerDisconnected,

	PLATC_NUM_EVENTS
} platC_SMA_signals_t;

/* -------------------------------- Events --------------------------------- */

/* ------------------------- Declares Active Objects ----------------------- */
RKH_SMA_DCLR(platC);

/* ------------------------------- Data types ------------------------------ */
typedef struct
{
	/* Data buffer */
	float ax_mean;
	float ax_var;
	float ax_min;
	float ax_max;
	float ay_mean;
	float ay_var;
	float ay_min;
	float ay_max;
	float az_mean;
	float az_var;
	float az_min;
	float az_max;
} MPU9250_samplerData_t;

struct platC_sampler_packet
{
	MPU9250_samplerData_t values;
	rui32_t time;
};

struct platC_packet
{
	MPU9250_data_t values;
	rui32_t time;
};

struct platC_config
{
	rui16_t motionThr;
	rbool_t buzzerEn;
	rui8_t advName[ADVERTISE_NAME_SIZE_MAX+1];
};

struct platC_fitness
{
	rbool_t fitnessFlag;
};

typedef struct 
{
	RKH_EVT_T e;
	struct platC_packet data;
} evt_motion_t;

typedef struct
{
	RKH_EVT_T e;
	struct platC_config data;
} evt_config_t;

typedef struct
{
	RKH_EVT_T e;
	struct platC_fitness data;
} evt_fitness_t;
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */

#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif
/* ------------------------------ File footer ------------------------------ */
