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
 *  \file       platC_cmds.c
 *  \brief      PlatC commands for PlatC Demo.
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
#include "platC_cmds.h"

/* ----------------------------- Local macros ------------------------------ */
/* ---------------------------- Local data types --------------------------- */
/* BLE commands parser handler */
typedef void (*cmdProcessor_t)(const uint8_t seqNo, const uint8_t * const data, const uint8_t size);

struct platC_cmds
{
	uint8_t        cmdId;
	cmdProcessor_t pfnProcessor;
};

/* ------------------------------- Constants ------------------------------- */

/* Include below file functions handlers */
#include "platC_cmds_handlers.h"

const struct platC_cmds g_platCCommandsArr[] =
{
		   /* CMD_ID */				/* PFN_PROCESSOR */
	{	PLATC_CMD_ID_GET_DATA	, platC_getDataCmd_proc		}
,   {	PLATC_CMD_ID_CONFIG		, platC_configCmd_proc		}
,	{	PLATC_CMD_ID_GET_CONFIG	, platC_getConfigCmd_proc	}
,	{	PLATC_CMD_ID_FITNESS	, platC_fitnessCmd_proc		}
,	{	PLATC_CMD_ID_MAX		,			0				}
};

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
void bgm113_cmds_parser_indDataHandler(const rui8_t seqNo, const rui8_t * const data, const rui8_t size)
{
	rui32_t cmdId;

	/* Try get cmdId from Compact JSON payload*/
	cmdId = jRead_int((char *)data, "{'cmd'", NULL);

	/* Check if cmdId processor handler exists and invoke function callback */
	for(rui8_t i = 0;PLATC_CMD_ID_MAX > g_platCCommandsArr[i].cmdId;i++)
	{
		if(cmdId == g_platCCommandsArr[i].cmdId)
		{
			if(NULL != g_platCCommandsArr[i].pfnProcessor)
				g_platCCommandsArr[i].pfnProcessor(seqNo, data, size);
		}
	}
}
/* ------------------------------ End of file ------------------------------ */
