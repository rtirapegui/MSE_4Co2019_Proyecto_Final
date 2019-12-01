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
 *  \file       platC_cmds_handlers .c
 *  \brief      PlatC commands handlers for PlatC Demo.
 *
 *  \ingroup    app
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.07.30  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "bsp.h"
#include "jRead.h"
#include "platC.h"
#include "platC_cmds_handlers.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
static RKH_ROM_STATIC_EVENT(e_platCGetCfg, evPlatCGetCfg);
static RKH_ROM_STATIC_EVENT(e_platCGetData, evPlatCGetData);
static RKH_ROM_STATIC_EVENT(e_platCMotionDetect, evMotionDetect);
static evt_config_t e_cfg;
static evt_fitness_t e_fitness;
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
 #if defined(RKH_USE_TRC_SENDER)
	 static rui8_t cmds;
 #endif
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
/* PLATC_CMD_ID_ACK */

/* PLATC_CMD_ID_DATA */

/* PLATC_CMD_ID_GET_DATA */
void platC_getDataCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size)
{
	RKH_SMA_POST_FIFO(platC, CE(&e_platCGetData), &cmds);
}

/* PLATC_CMD_ID_CONFIG */
void platC_configCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size)
{
	/* Get motion threshold value */
	e_cfg.data.motionThr = jRead_int((char *)data, "{'h'", NULL);

	/* Get buzzer enable status */
	e_cfg.data.buzzerEn = jRead_int((char *)data, "{'b'", NULL);
	
	/* Get advertise name value */
	jRead_string((char *)data, (char *) "{'n'", (char *) e_cfg.data.advName, ADVERTISE_NAME_SIZE_MAX+1, NULL);

	RKH_SET_STATIC_EVENT(&e_cfg, evPlatCCfg);
	RKH_SMA_POST_FIFO(platC, CE(&e_cfg), &cmds);
}

/* PLATC_CMD_ID_GET_CONFIG */
void platC_getConfigCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size)
{
	RKH_SMA_POST_FIFO(platC, CE(&e_platCGetCfg), &cmds);
}

/* PLATC_CMD_ID_FITNESS */
void platC_fitnessCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size)
{
	/* Get fitness flag value */
	e_fitness.data.fitnessFlag = jRead_int((char *)data, "{'f'", NULL);

	RKH_SET_STATIC_EVENT(&e_fitness, evPlatCFitness);
	RKH_SMA_POST_FIFO(platC, CE(&e_fitness), &cmds);
}

/* PLATC_CMD_ID_MOTION_DETECTED */
void platC_motionDetectedCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size)
{
	RKH_SMA_POST_FIFO(platC, CE(&e_platCMotionDetect), &cmds);
}
/* ------------------------------ End of file ------------------------------ */