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
 *  \file       uart0.h
 *  \brief      UART Driver of SERCOM 0 Header for PlatC
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.07.16  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */

 /* --------------------------------- Module -------------------------------- */
#ifndef UART0_H_
#define UART0_H_

/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "conf_sercoms.h"

/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */

/* Valid buffer size value are only power of 2 (ex: 2,4,..,64,128) */
#if defined(UART0_RX_BUFFER_SIZE)>0
#	define UART0_RX_INTERRUPT_DRIVEN_MODE
#else
#	define UART0_RX_POLLED_MODE
#endif

#if defined(UART0_TX_BUFFER_SIZE)>0
#	define UART0_TX_INTERRUPT_DRIVEN_MODE
#else
#	define UART0_TX_POLLED_MODE
#endif

/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
rbool_t uart0_init(void);
rbool_t uart0_enable(void);
rbool_t uart0_disable(void);
rbool_t uart0_readByteNoWait(uint8_t * byte);
rui8_t uart0_readData(rui8_t * buff, const rui8_t expBytesLen, const rui32_t timeout);
rbool_t uart0_writeByte(const rui8_t byte);
rbool_t uart0_writeData(const rui8_t *buff, const rui32_t len);

/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ File footer ------------------------------ */