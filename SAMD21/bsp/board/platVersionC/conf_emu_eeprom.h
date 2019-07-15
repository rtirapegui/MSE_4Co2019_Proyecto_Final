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
 *  \file       conf_emu_eeprom.h
 *  \brief      Emulated Eeprom Memory Layout for PlatC.
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
#ifndef CONF_EMU_EEPROM_H_
#define CONF_EMU_EEPROM_H_

/* ----------------------------- Include files ----------------------------- */
#include "preprocessor.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
// Application Non-volatile emulated EEPROM memory layout

/* Emulated eeprom layout */
#define DECLARE_ID(n, unsused)			EMU_EEPROM_ID_##n,
#define DECLARE_ID_ALIAS(id)			EMU_EEPROM_ID_##id

// Set the maximum number of elements inside the eeprom
#define EMU_EEPROM_ID_MAX				3

/* Define below the size of each element present in the eeprom. Use the format EMU_EEPROM_SIZE_ID_x where x is [0 - EMU_EEPROM_ID_MAX - 1] */

/*  configuration parameters */
#define EMU_EEPROM_SIZE_ID_0			0x02		// MOTION_THRESHOLD
#define EMU_EEPROM_SIZE_ID_1			0x15		// ADVERTISE_NAME
#define EMU_EEPROM_SIZE_ID_2			0x01		// BUZZER_ENABLED

typedef enum
{
	MREPEAT(EMU_EEPROM_ID_MAX,DECLARE_ID,~)
} emu_eeprom_id_t;

/* Define below the alias of each element present in the eeprom. Those alias are assigned to each EMU_EEPROM_ID by using DECLARE_ID_ALIAS macro */

/* LIGHT WEIGHT MESH configuration parameters */
#define EMU_EEPROM_MOTION_THRESHOLD		DECLARE_ID_ALIAS(0)
#define EMU_EEPROM_ADVERTISE_NAME		DECLARE_ID_ALIAS(1)
#define EMU_EEPROM_BUZZER_ENABLED		DECLARE_ID_ALIAS(2)

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
