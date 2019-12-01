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
 *  \file       platC_cmds_handlers.h
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

 /* --------------------------------- Module -------------------------------- */
#ifndef _PLAT_C_CMDS_HANDLERS_H_
#define _PLAT_C_CMDS_HANDLERS_H_

/* ----------------------------- Include files ----------------------------- */

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* -------------------------------- Signals -------------------------------- */
/* -------------------------------- Events --------------------------------- */
/* ------------------------- Declares Active Objects ----------------------- */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */

/* PLATC_CMD_ID_ACK */

/* PLATC_CMD_ID_DATA */

/* PLATC_CMD_ID_GET_DATA */
void platC_getDataCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size);

/* PLATC_CMD_ID_CONFIG */
void platC_configCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size);

/* PLATC_CMD_ID_GET_CONFIG */
void platC_getConfigCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size);

/* PLATC_CMD_ID_FITNESS */
void platC_fitnessCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size);

/* PLATC_CMD_ID_MOTION_DETECTED */
void platC_motionDetectedCmd_proc(const uint8_t seqNo, const uint8_t * const data, const uint8_t size);

/* -------------------- External C language linkage end -------------------- */

#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif
/* ------------------------------ File footer ------------------------------ */
