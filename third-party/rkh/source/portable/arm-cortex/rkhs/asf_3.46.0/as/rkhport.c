/*
 *  --------------------------------------------------------------------------
 *
 *                                Framework RKH
 *                                -------------
 *
 *            State-machine framework for reactive embedded systems
 *
 *                      Copyright (C) 2010 Leandro Francucci.
 *          All rights reserved. Protected by international copyright laws.
 *
 *
 *  RKH is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any
 *  later version.
 *
 *  RKH is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with RKH, see copying.txt file.
 *
 *  Contact information:
 *  RKH web site:   http://sourceforge.net/projects/rkh-reactivesys/
 *  e-mail:         francuccilea@gmail.com
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file       rkhport.c
 * 	\brief 		ARM Cortex-M0 MCU's,  ASF 3.46.0 AS
 *
 *  \ingroup    port
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.05.11  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  francuccilea@gmail.com
 *  RoTi  Rodrigo Tirapegui	 rtirapeg@itba.edu.ar
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "rkh.h"
#include "rkhfwk_dynevt.h"
#include "bsp.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
RKH_MODULE_NAME(rkhport)
RKH_MODULE_VERSION(rkhport, 1.00)
RKH_MODULE_DESC(rkhport, "ARM Cortex-M0+, ASF 3.46.0 AS")

#define cpu_irq_is_enabled()    (__get_PRIMASK() == 0)
#define cpu_irq_enable()										\
								do {                            \
									__DMB();                    \
									__enable_irq();             \
								} while (0)
#define cpu_irq_disable()										\
								do {                            \
									__disable_irq();            \
									__DMB();                    \
								} while (0)

								/** \brief This macro is used to test fatal errors.
 *
 * The macro tests if the expression is false. If it is, a fatal error is
 * detected and the application hangs up. If \c TEST_SUITE_DEFINE_ASSERT_MACRO
 * is defined, a unit test version of the macro is used, to allow execution
 * of further tests after a false expression.
 *
 * \param[in] expr  Expression to evaluate and supposed to be nonzero.
 */
#if defined(_ASSERT_ENABLE_)
#	define Assert(expr)					\
		{								\
           if (!(expr)) asm("BKPT #0");	\
        }
#else
#	define Assert(expr) ((void) 0)
#endif

/* ---------------------------- Local data types --------------------------- */
static volatile ruint	cpu_irq_critical_section_counter;
static volatile rbool_t cpu_irq_prev_interrupt_state;

/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
const char * rkhport_get_version(void)
{
    return RKH_MODULE_GET_VERSION();
}

const char * rkhport_get_desc(void)
{
    return RKH_MODULE_GET_DESC();
}

void rkhport_enter_critical(void)
{
	if (cpu_irq_critical_section_counter == 0) {
		if (cpu_irq_is_enabled()) {
			cpu_irq_disable();
			cpu_irq_prev_interrupt_state = true;
			} else {
			/* Make sure the to save the prev state as false */
			cpu_irq_prev_interrupt_state = false;
		}

	}

	cpu_irq_critical_section_counter++;
}

void rkhport_exit_critical(void)
{
	/* Check if the user is trying to leave a critical section when not in a critical section */
	Assert(cpu_irq_critical_section_counter > 0);

	cpu_irq_critical_section_counter--;

	/* Only enable global interrupts when the counter reaches 0 and the state of the global interrupt flag
	   was enabled when entering critical state */
	if ((cpu_irq_critical_section_counter == 0) && (cpu_irq_prev_interrupt_state)) {
		cpu_irq_enable();
	}
}

/* ------------------------------ File footer ------------------------------ */
