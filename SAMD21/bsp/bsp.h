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
 *  \file       bsp.h
 *  \brief      Board Support Package for PlatC.
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.05.11  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */

/* --------------------------------- Module -------------------------------- */
#ifndef __BSP_H__
#define __BSP_H__

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "emu_eeprom.h"
#include "mpu9250.h"
#include "push_button.h"
#include "battery.h"
#include "bgm113.h"
#include "platCbsp.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
void bsp_init(void);
rui32_t bsp_getTimeSec(void);
rui8_t bsp_writeEepromData(emu_eeprom_id_t id, const rui8_t *data);
rui8_t bsp_readEepromData(emu_eeprom_id_t id, const rui8_t *data);
rbool_t bsp_setMotionThreshold(rui16_t womThreshold);

#define bsp_registerBatteryCb(x)		battery_registerCb(x);
#define bsp_registerAccelCb(x)			mpu9250_registerCb(x);
#define bsp_registerBleCb(x)			bgm113_registerCb(x);
#define bsp_startBleAdvertisement()		bgm113_startAdvertising();
#define bsp_stopBleAdvertisement()		bgm113_stopAdvertising();

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ File footer ------------------------------ */
