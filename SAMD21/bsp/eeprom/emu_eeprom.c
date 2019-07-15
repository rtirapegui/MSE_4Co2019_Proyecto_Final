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
 *  \file       emu_eeprom.c
 *  \brief      Emulated Eeprom Driver for PlatC.
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
 /* --------------------------------- Notes --------------------------------- */
 /* ----------------------------- Include files ----------------------------- */
 #include "emu_eeprom.h"
 #include "eeprom.h"
 #include "nvm.h"
 
 /* ----------------------------- Local macros ------------------------------ */
 #define DECLARE_ID_SIZE(n, unsused)	EMU_EEPROM_SIZE_ID_##n,
 /* ------------------------------- Constants ------------------------------- */
 const uint8_t emuEepromIdSize[EMU_EEPROM_ID_MAX] =
 { 
	MREPEAT(EMU_EEPROM_ID_MAX,DECLARE_ID_SIZE,~)
 };
 /* ---------------------------- Local Variables ---------------------------- */
 static rui8_t g_initFlag;
 /* ---------------------------- Local data types --------------------------- */
 /* ----------------------- Local function prototypes ----------------------- */
 /* ---------------------------- Local functions ---------------------------- */
 static rui16_t getTagOffset(emu_eeprom_id_t id)
 {
	 rui16_t offset = 0;
	 rui8_t  i;

	 for(i = 0;i < id;i++)
		offset += emuEepromIdSize[i];

	 return offset;
 }
 static rbool_t emu_eeprom_getMemorySize(rui32_t *bytesLen)
 {
	  if(bytesLen)
	  {
		  struct eeprom_emulator_parameters params;

		  if(STATUS_OK == eeprom_emulator_get_parameters(&params))
		  {
			  *bytesLen = ((rui32_t) params.eeprom_number_of_pages) * ((rui32_t) params.page_size);

			  return true;
		  }
	  }
	  
	  return false;
 }

 /* ---------------------------- Global functions --------------------------- */
 rbool_t emu_eeprom_init(void)
 {
	  enum status_code error_code;
	  
	  error_code = eeprom_emulator_init();		// Setup EEPROM emulator service

	  if(error_code != STATUS_ERR_NO_MEMORY)		// Fail if NVM_EMULATED_EEPROM fuses are not enabled
	  {
		  rui32_t emuEepromSize;

		  if(error_code != STATUS_OK)
		  {
			  eeprom_emulator_erase_memory();
			  eeprom_emulator_init();
		  }
		  
		  if(emu_eeprom_getMemorySize(&emuEepromSize))
		  {
			  rui8_t  i;
			  rui32_t emuEepromUserFootprint = 0;

			  for(i = 0;i < EMU_EEPROM_ID_MAX;i++)
				emuEepromUserFootprint += emuEepromIdSize[i];

			  if(emuEepromUserFootprint <= emuEepromSize)
			  {
				  g_initFlag = true;
				  
				  return true;
			  }
		  }
	  }

	  return false;
 }
 rui8_t emu_eeprom_writeData(emu_eeprom_id_t id, const rui8_t *data)
 {
	  if(g_initFlag && data && (id < EMU_EEPROM_ID_MAX))
	  {
		  if(STATUS_OK == eeprom_emulator_write_buffer(getTagOffset(id), data, emuEepromIdSize[id]))
		  {
			 eeprom_emulator_commit_page_buffer();
			 return emuEepromIdSize[id];
		  }
	  }

	  return 0;
 }
 rui8_t emu_eeprom_readData(emu_eeprom_id_t id, const rui8_t *data)
 {
	if(g_initFlag && data && (id < EMU_EEPROM_ID_MAX))
	{
		if(STATUS_OK == eeprom_emulator_read_buffer(getTagOffset(id), (rui8_t *) data, emuEepromIdSize[id]))
			return emuEepromIdSize[id];
	}

	return 0;
 }
 void emu_eeprom_eraseFullMemory(void)
 {
	eeprom_emulator_erase_memory();
 }

 /* ------------------------------ End of file ------------------------------ */