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
 *  \file       platC.c
 *  \brief      PlatC SMA for PlatC Demo.
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
 /* --------------------------------- Notes --------------------------------- */
 /* ----------------------------- Include files ----------------------------- */
 #include <string.h>
 #include "arm_math.h"
 #include "jRead.h"
 #include "jWrite.h"
 #include "platC_cmds.h"
 #include "platC.h"

 /* ----------------------------- Local macros ------------------------------ */
 #define UNLINKED_TIMEOUT        RKH_TIME_SEC(60)
 #define MOTION_DET_TIME         RKH_TIME_MS(3000)
 #define KEEP_ALIVE_TIME		 RKH_TIME_MS(500)
 #define ACCEL_WINDOW_TIME		 RKH_TIME_MS(1000)
 #define SHORT2LONG_BUTT_TIME    RKH_TIME_MS(3000)
 #define DEBOUNCE_SET_MASK		 0xFF
 #define DEBOUNCE_UNSET_MASK	 ((~DEBOUNCE_SET_MASK) & 0xFF)

 typedef enum
 {
	 stReleased,
	 stPress,
	 stLongPress
 } push_button_state_t;

 typedef enum
  {
	  stDisconnected,
	  stConnected
  } bgm113_state_t;

 typedef enum
  {
	stNotCharging,
	stCharging
  } battery_state_t;
 
 /* ------------------------- Declares active object ------------------------ */
 typedef struct PlatC PlatC;
 typedef struct platC_control platC_control_t;
 /* ------------------- Declares states and pseudostates -------------------- */
 RKH_DCLR_COMP_STATE   steady;
 RKH_DCLR_BASIC_STATE  unlinked, linked, hidden, motionDetect;
 RKH_DCLR_SHIST_STATE  steadySHist;

 /* ------------------------ Declares initial action ------------------------ */
  static void init(PlatC *const me, RKH_EVT_T *pe);
 /* ------------------------ Declares effect actions ------------------------ */
  static void sendMotionByButton(PlatC *const me, RKH_EVT_T *pe);
  static void sendMotionByCmd(PlatC *const me, RKH_EVT_T *pe);
  static void updateCfg(PlatC *const me, RKH_EVT_T *pe);
  static void sendCfg(PlatC *const me, RKH_EVT_T *pe);
  static void updateFitness(PlatC *const me, RKH_EVT_T *pe);
  static void setBatteryChargingStatus(PlatC *const me, RKH_EVT_T *pe);
  static void unsetBatteryChargingStatus(PlatC *const me, RKH_EVT_T *pe);
 /* ------------------------- Declares entry actions ------------------------ */
  static void linkedEntry(PlatC *const me);
  static void unlinkedEntry(PlatC *const me);
  static void hiddenEntry(PlatC *const me);
  static void motionEntry(PlatC *const me);
 /* ------------------------- Declares exit actions ------------------------- */
  static void linkedExit(PlatC *const me);
  static void unlinkedExit(PlatC *const me);
  static void hiddenExit(PlatC *const me);
  static void motionExit(PlatC *const me);
 /* ---------------------------- Declares guards ---------------------------- */
 /* ------------------------ States and pseudostates ------------------------ */
 RKH_CREATE_COMP_STATE(steady, NULL, NULL, RKH_ROOT, &unlinked, &steadySHist);
 RKH_CREATE_TRANS_TABLE(steady)
	 RKH_TRINT(evPlatCCfg, NULL, &updateCfg),
	 RKH_TRINT(evPlatCGetCfg, NULL, &sendCfg),
	 RKH_TRREG(evMotionDetect, NULL, NULL, &motionDetect),
	 RKH_TRINT(evBatteryChargerConnected, NULL, &setBatteryChargingStatus),
	 RKH_TRINT(evBatteryChargerDisconnected, NULL, &unsetBatteryChargingStatus),
 RKH_END_TRANS_TABLE

 RKH_CREATE_SHALLOW_HISTORY_STATE(steadySHist, &steady, NULL, NULL, NULL);
 
 RKH_CREATE_BASIC_STATE(unlinked, unlinkedEntry, unlinkedExit, &steady, NULL);
 RKH_CREATE_TRANS_TABLE(unlinked)
 	 RKH_TRINT(evBatteryChargerConnected, NULL, &setBatteryChargingStatus),
 	 RKH_TRINT(evBatteryChargerDisconnected, NULL, &unsetBatteryChargingStatus),
	 RKH_TRREG(evUnlikedTout, NULL, NULL, &hidden),
	 RKH_TRREG(evConnected, NULL, NULL, &linked),
 RKH_END_TRANS_TABLE

 RKH_CREATE_BASIC_STATE(hidden, hiddenEntry, hiddenExit, &steady, NULL);
 RKH_CREATE_TRANS_TABLE(hidden)
 	RKH_TRINT(evBatteryChargerConnected, NULL, &setBatteryChargingStatus),
 	RKH_TRINT(evBatteryChargerDisconnected, NULL, &unsetBatteryChargingStatus),
	RKH_TRREG(evPushbuttonLongPress, NULL, NULL, &unlinked),
 RKH_END_TRANS_TABLE

 RKH_CREATE_BASIC_STATE(linked, linkedEntry, linkedExit, &steady, NULL);
 RKH_CREATE_TRANS_TABLE(linked)
	 RKH_TRINT(evBatteryChargerConnected, NULL, &setBatteryChargingStatus),
	 RKH_TRINT(evBatteryChargerDisconnected, NULL, &unsetBatteryChargingStatus),
	 RKH_TRINT(evPlatCGetData, NULL, &sendMotionByCmd),
	 RKH_TRINT(evPlatCCfg, NULL, &updateCfg),
	 RKH_TRINT(evPlatCFitness, NULL, &updateFitness),
	 RKH_TRREG(evMotionDetect, NULL, NULL, &motionDetect),
	 RKH_TRREG(evPushbuttonShortPress, NULL, sendMotionByButton, &motionDetect),
	 RKH_TRREG(evPushbuttonLongPress, NULL, NULL, &unlinked),
	 RKH_TRREG(evDisconnected, NULL, NULL, &unlinked),
 RKH_END_TRANS_TABLE

 RKH_CREATE_BASIC_STATE(motionDetect, motionEntry, motionExit, RKH_ROOT, NULL);
 RKH_CREATE_TRANS_TABLE(motionDetect)
	 RKH_TRINT(evPlatCCfg, NULL, &updateCfg),
	 RKH_TRINT(evPlatCGetCfg, NULL, &sendCfg),
	 RKH_TRREG(evMotionIndicatorTout, NULL, NULL, &steadySHist),
 RKH_END_TRANS_TABLE

 /* ----------------------------- Active object ----------------------------- */
 struct PlatC
 {
	 RKH_SMA_T sma;
	 RKH_TMR_T tmr;
 };

 RKH_SMA_CREATE(PlatC, platC, 0, HCAL, &unlinked, init, NULL);
 RKH_SMA_DEF_PTR(platC);

 /* ------------------------------- Constants ------------------------------- */
 RKH_MODULE_NAME(platCModule)
 #define JSON_BUFFER_SIZE_MAX	150
 /* ---------------------------- Local Variables ---------------------------- */
 #if defined(RKH_USE_TRC_SENDER)
	static rui8_t pushbutton;
	static rui8_t ble;
	static rui8_t accel;
	static rui8_t battery;
 #endif

 static RKH_STATIC_EVENT(e_tmr, evInitTout);
 static RKH_STATIC_EVENT(e_pushbutton, evPushbuttonLongPress);
 static RKH_STATIC_EVENT(e_ble, evOk);
 static RKH_STATIC_EVENT(e_battery, evBatteryChargerConnected);
 
 static platC_control_t g_controlData;
 static rui8_t jsonBuff[JSON_BUFFER_SIZE_MAX];
 /* ---------------------------- Local data types --------------------------- */
 struct platC_control
 {
	rbool_t isConnected;
	rbool_t isMotionDetect;
	rbool_t isBatteryCharging;
	rbool_t isAdvertising;
	rbool_t isBuzzerEnabled;
	rbool_t isFitnessRunning;
	rui16_t motionThreshold;
	rui8_t  advName[ADVERTISE_NAME_SIZE_MAX+1];
 };
 /* ----------------------- Local function prototypes ----------------------- */
 /* ---------------------------- Local functions ---------------------------- */
 static char * formatPlatCSamplerData(struct platC_sampler_packet *p)
  {
	  jwOpen((char *) jsonBuff, sizeof(jsonBuff), JW_OBJECT, JW_COMPACT);
	  jwObj_int("cmd", PLATC_CMD_ID_SAMPLER_DATA);
	  jwObj_int("t", p->time);
	  jwObj_double("xm", p->values.ax_mean);
	  jwObj_double("ym", p->values.ay_mean);
	  jwObj_double("zm", p->values.az_mean);
	  jwObj_double("xv", p->values.ax_var);
	  jwObj_double("yv", p->values.ay_var);
	  jwObj_double("zv", p->values.az_var);
	  //jwObj_double("xa", p->values.ax_max);
	  //jwObj_double("ya", p->values.ay_max);
	  //jwObj_double("za", p->values.az_max);
	  //jwObj_double("xa", p->values .ax_min);
	  //jwObj_double("ya", p->values.ay_min);
	  //jwObj_double("za", p->values.az_min);
	  jwClose();

	  return (char *) jsonBuff;
  }
 static char * formatPlatCData(struct platC_packet *p, push_button_state_t bst)
  {
	  jwOpen((char *) jsonBuff, sizeof(jsonBuff), JW_OBJECT, JW_COMPACT);
	  jwObj_int("cmd", PLATC_CMD_ID_DATA);
	  jwObj_int("t", p->time);
	  jwObj_double("x", p->values.ax);
	  jwObj_double("y", p->values.ay);
	  jwObj_double("z", p->values.az);
	  jwObj_double("u", p->values.gx);
	  jwObj_double("v", p->values.gy);
	  jwObj_double("w", p->values.gz);
	  jwObj_double("c", p->values.t);
	  jwObj_int("b", bst);
	  jwObj_int("h", g_controlData.motionThreshold);
	  jwObj_int("e", g_controlData.isBuzzerEnabled);
	  jwObj_string("n", (char *) g_controlData.advName);
	  jwClose();

	  return (char *) jsonBuff;
  }
 static char * formatPlatCAck(void)
  {
	  jwOpen((char *) jsonBuff, sizeof(jsonBuff), JW_OBJECT, JW_COMPACT );
	  jwObj_int("cmd", PLATC_CMD_ID_ACK);
	  jwClose();

	  return (char *) jsonBuff;
  }
 static char * formatPlatCCfg(void)
  {
	  jwOpen((char *) jsonBuff, sizeof(jsonBuff), JW_OBJECT, JW_COMPACT );
	  jwObj_int("cmd", PLATC_CMD_ID_GET_CONFIG);
	  jwObj_int("h", g_controlData.motionThreshold);
	  jwObj_int("e", g_controlData.isBuzzerEnabled);
	  jwObj_string("n", (char *) g_controlData.advName);
	  jwClose();

	  return (char *) jsonBuff;
  }
 static void accel_sampler(void)
 {
	/* Get in here every RKH_TICK_RATE_MS */
	#define ACCEL_SAMPLES_MAX	ACCEL_WINDOW_TIME	// It's normalized by RKH_TICK_RATE_MS
	static rui32_t sampleIndex, accelWindowTime = ACCEL_WINDOW_TIME;
	static float axSampleBuf[ACCEL_SAMPLES_MAX], aySampleBuf[ACCEL_SAMPLES_MAX], azSampleBuf[ACCEL_SAMPLES_MAX];
	//static float gxSampleBuf[ACCEL_SAMPLES_MAX], gySampleBuf[ACCEL_SAMPLES_MAX], gzSampleBuf[ACCEL_SAMPLES_MAX];

	if((g_controlData.isConnected) && (g_controlData.isFitnessRunning))
	{
		MPU9250_data_t data = bsp_getMotionData();

		axSampleBuf[sampleIndex%ACCEL_SAMPLES_MAX] = data.ax;
		aySampleBuf[sampleIndex%ACCEL_SAMPLES_MAX] = data.ay;
		azSampleBuf[sampleIndex%ACCEL_SAMPLES_MAX] = data.az;
		//gxSampleBuf[sampleIndex%ACCEL_SAMPLES_MAX] = data.gx;
		//gySampleBuf[sampleIndex%ACCEL_SAMPLES_MAX] = data.gy;
		//gzSampleBuf[sampleIndex%ACCEL_SAMPLES_MAX] = data.gz;

		sampleIndex++;

		if(accelWindowTime && --accelWindowTime == 0)
		{
			struct platC_sampler_packet packet;
			uint32_t index;

			accelWindowTime = ACCEL_WINDOW_TIME;
			
			/* Process accel samples */

			/* Calculate mean by axis */
			arm_mean_f32(axSampleBuf, sizeof(axSampleBuf)/sizeof(axSampleBuf[0]), &packet.values.ax_mean);
			arm_mean_f32(aySampleBuf, sizeof(aySampleBuf)/sizeof(aySampleBuf[0]), &packet.values.ay_mean);
			arm_mean_f32(azSampleBuf, sizeof(azSampleBuf)/sizeof(azSampleBuf[0]), &packet.values.az_mean);
			
			/* Calculate variance by axis */
			arm_var_f32(axSampleBuf, sizeof(axSampleBuf)/sizeof(axSampleBuf[0]), &packet.values.ax_var);
			arm_var_f32(aySampleBuf, sizeof(aySampleBuf)/sizeof(aySampleBuf[0]), &packet.values.ay_var);
			arm_var_f32(azSampleBuf, sizeof(azSampleBuf)/sizeof(azSampleBuf[0]), &packet.values.az_var);
			
			/* Calculate max by axis */
			arm_max_f32(axSampleBuf, sizeof(axSampleBuf)/sizeof(axSampleBuf[0]), &packet.values.ax_max, &index);
			arm_max_f32(aySampleBuf, sizeof(aySampleBuf)/sizeof(aySampleBuf[0]), &packet.values.ay_max, &index);
			arm_max_f32(azSampleBuf, sizeof(azSampleBuf)/sizeof(azSampleBuf[0]), &packet.values.az_max, &index);

			/* Calculate min by axis */
			arm_min_f32(axSampleBuf, sizeof(axSampleBuf)/sizeof(axSampleBuf[0]), &packet.values.ax_min, &index);
			arm_min_f32(aySampleBuf, sizeof(aySampleBuf)/sizeof(aySampleBuf[0]), &packet.values.ay_min, &index);
			arm_min_f32(azSampleBuf, sizeof(azSampleBuf)/sizeof(azSampleBuf[0]), &packet.values.az_min, &index);

			packet.time = bsp_getTimeSec();

			bsp_sendBleData((const rui8_t *) formatPlatCSamplerData(&packet));
		}
	}
	else
	{
		if(ACCEL_WINDOW_TIME != accelWindowTime)
			accelWindowTime = ACCEL_WINDOW_TIME;

		if(sampleIndex)
			sampleIndex = 0;
	}
 }
 static void push_button_SM(void)
 {
	 static rui32_t state, filter, button, tout;
 
	 /* Debouncer filter */
	 filter = (filter << 1) | PUSH_BUTTON_GET_STATUS();

	 if(DEBOUNCE_SET_MASK == filter)
	 {
		button = stPress;	
	 }
	 else if(DEBOUNCE_UNSET_MASK == filter)
	 {
		button = stReleased;
	 }

	 /* Pushbutton state machine */
	 switch(state)
	 {
		 case stReleased:
		 {
			 if(stPress == button)
			 {
				 state = stPress;
				 tout = SHORT2LONG_BUTT_TIME;
			 }
		 } break;

		 case stPress:
		 {
			 if(stReleased == button)
			 {
			     /* Set event push button short press */
				 RKH_SET_STATIC_EVENT(&e_pushbutton, evPushbuttonShortPress);
				 
				 /* Post push button event to platC object */
				 RKH_SMA_POST_FIFO(platC, &e_pushbutton, &pushbutton);
				 
				 state = stReleased;
			 }
			 else if(tout && (--tout == 0))
			 {
				 /* Set event push button long press */
				 RKH_SET_STATIC_EVENT(&e_pushbutton, evPushbuttonLongPress);
				 
				 /* Post push button event to platC object */
				 RKH_SMA_POST_FIFO(platC, &e_pushbutton, &pushbutton);
				 
				 state = stLongPress;
			 }
		 } break;

		 case stLongPress:
		 {
			if(stReleased == button)
				state = stReleased;
		 } break;

		 default:
		 {
			state = stReleased;
		 } break;
	 }
 }
 static void ble_connection_SM(void)
 {
	 static rui32_t state, filter, ble;
	 
	 /* Debouncer filter */
	 filter = (filter << 1) | BLE_STATUS_GET_STATUS();

	if(DEBOUNCE_SET_MASK == filter)
	{
		ble = stConnected;
	}
	else if(DEBOUNCE_UNSET_MASK == filter)
	{
		ble = stDisconnected;
	}

	 /* Ble state machine */
	 switch(state)
	 {
		 case stDisconnected:
		 {
			 if(stConnected == ble)
			 {
				  /* Set event ble connected */
				  RKH_SET_STATIC_EVENT(&e_ble, evConnected);

				   /* Post ble event to platC object */
				  RKH_SMA_POST_FIFO(platC, &e_ble, &ble);

				  state = stConnected;
			 }
		 } break;

		 case stConnected:
		 {
			 if(stDisconnected == ble)
			 {
				  /* Set event ble disconnected */
				  RKH_SET_STATIC_EVENT(&e_ble, evDisconnected);

				  /* Post ble event to platC object */
				  RKH_SMA_POST_FIFO(platC, &e_ble, &ble);

				  state = stDisconnected;
			 }
		 } break;

		 default:
		 {
			 state = stDisconnected;
		 } break;
	 }
 }
 static void battery_charging_SM(void)
 {
	 static rui32_t state, filter, battery;
	 
	 /* Debouncer filter */
	 filter = (filter << 1) | BATTERY_STATUS_GET_STATUS();

	 if(DEBOUNCE_SET_MASK == filter)
	 {
		 battery = stCharging;
	 }
	 else if(DEBOUNCE_UNSET_MASK == filter)
	 {
		 battery = stNotCharging;
	 }

	 /* Battery state machine */
	 switch(state)
	 {
		 case stNotCharging:
		 {
			 if(stCharging == battery)
			 {
				 /* Set event battery charger connected */
				 RKH_SET_STATIC_EVENT(&e_battery, evBatteryChargerConnected);

				 /* Post battery event to platC object */
				 RKH_SMA_POST_FIFO(platC, &e_battery, &battery);

				 state = stCharging;
			 }
		 } break;

		 case stCharging:
		 {
			 if(stNotCharging == battery)
			 {
				 /* Set event battery charger disconnected */
				 RKH_SET_STATIC_EVENT(&e_battery, evBatteryChargerDisconnected);

				 /* Post battery event to platC object */
				 RKH_SMA_POST_FIFO(platC, &e_battery, &battery);

				 state = stNotCharging;
			 }
		 } break;

		 default:
		 {
			 state = stNotCharging;
		 } break;
	 }
 }
 static void setLedsStatus(void)
 {
	if(g_controlData.isMotionDetect)
	{
		/* Set motion detection led over ble and battery led */

		/* Turn on motion detected led */
		LED_On(MOTION_LED_PIN);

		/* Turn off battery charging led */
		LED_Off(BATTERY_LED_PIN);

		/* Turn off ble connected led */
		LED_Off(BLE_LED_PIN);
	}
	else
	{
		/* Turn off motion detected led */
		LED_Off(MOTION_LED_PIN);

		/* Set ble connection led status */
		if(g_controlData.isConnected)
		{
			/* Turn on ble connected led */
			LED_On(BLE_LED_PIN);

			/* Turn off battery connected led */
			LED_Off(BATTERY_LED_PIN);
		}
		else
		{
			/* Turn off battery connected led */
			LED_Off(BLE_LED_PIN);

			/* Set battery charging led status */
			g_controlData.isBatteryCharging ? LED_On(BATTERY_LED_PIN) : LED_Off(BATTERY_LED_PIN);
		}
	}
 }
 static void setBuzzer(void)
 {
	/* Turn on buzzer when motion is detected */
	if(g_controlData.isBuzzerEnabled)
		bsp_buzzerOn();
 }
 static void unsetBuzzer(void)
 {
	 /* Turn off buzzer once motion has ended */
	 bsp_buzzerOff();
 }
 void rkh_hook_timetick(void)
 {	
	 /* Get in here every RKH_TICK_RATE_MS */
	 static rui32_t ledIndex, keepAliveTime = KEEP_ALIVE_TIME;
	 
	 /* Invoke accelerometer sampler */
	 accel_sampler();

	 /* Invoke push button state machine */
	 push_button_SM();

	 /* Invoke ble status state machine */
	 ble_connection_SM();

	 /* Invoke battery status state machine */
	 battery_charging_SM();
	 	 
	 /* Toggle MOTION, BLE and BATTERY LED every KEEP_ALIVE_TIME */
	 if(!g_controlData.isConnected && !g_controlData.isBatteryCharging && !g_controlData.isMotionDetect)
	 {
		if(g_controlData.isAdvertising)
		{
			if(keepAliveTime && --keepAliveTime == 0)
			{
				keepAliveTime = KEEP_ALIVE_TIME;
			
				switch((ledIndex++)%3)
				{
					case 0:	LED_Toggle(MOTION_LED_PIN);		break;
					case 1:	LED_Toggle(BLE_LED_PIN);		break;
					case 2:	LED_Toggle(BATTERY_LED_PIN);	break;
					default:	ledIndex = 0;				break;
				}
			}
		}
		else
		{
			LED_Off(MOTION_LED_PIN);
			LED_Off(BLE_LED_PIN);
			LED_Off(BATTERY_LED_PIN);
		}
	 }

	/* Stay in STANDBY sleep until low voltage is detected */
	//system_sleep();
 }
 void rkh_hook_idle(void)
 {
	/* Needed by RKH framework */
	RKH_ENA_INTERRUPT();
	RKH_TRC_FLUSH();

	if(g_controlData.isConnected)
	{	// Process BLE incoming data once per second
		static rui32_t prevTimeSec;
		rui32_t currTimeSec = bsp_getTimeSec();

		if(currTimeSec != prevTimeSec)
		{
			/* Invoke ble processing task */
			bsp_processBleIncomingData(currTimeSec * 1000u);
			prevTimeSec = currTimeSec;
		}
	}
	// @todo.rtirapegui.30.07.2019 else sleep
 }
  
 /* ---------------------------- Initial action ----------------------------- */
 static void loadEepromData(void)
 {
 	rui32_t index;

 	if(!bsp_readEepromData(EMU_EEPROM_MOTION_THRESHOLD, (rui8_t *) &g_controlData.motionThreshold))
 		RKH_ERROR();

 	if(!bsp_readEepromData(EMU_EEPROM_ADVERTISE_NAME, (rui8_t *) &g_controlData.advName))
 		RKH_ERROR();

 	if(!bsp_readEepromData(EMU_EEPROM_BUZZER_ENABLED, (rui8_t *) &g_controlData.isBuzzerEnabled))
 		RKH_ERROR();

 	/* Check eeprom data sanity */
 	if((MOTION_THRESHOLD_MIN > g_controlData.motionThreshold) || (g_controlData.motionThreshold > MOTION_THRESHOLD_MAX))
 		g_controlData.motionThreshold = MOTION_THRESHOLD_DEFAULT;

 	if((0 != g_controlData.isBuzzerEnabled) && (1 != g_controlData.isBuzzerEnabled))
 		g_controlData.isBuzzerEnabled = BUZZER_ENABLED_DEFAULT;

 	for(index = 0;index < ADVERTISE_NAME_SIZE_MAX;index++)
 	{
	 	if(0xFF != g_controlData.advName[index])
	 		break;
 	}

 	if(ADVERTISE_NAME_SIZE_MAX <= index)
 		memcpy(g_controlData.advName, ADVERTISE_NAME_DEFAULT, min(sizeof(g_controlData.advName), sizeof(ADVERTISE_NAME_DEFAULT)));
 }
 static void init(PlatC *const me, RKH_EVT_T *pe)
 {
	/* Load eeprom data */
	loadEepromData();

	RKH_TMR_INIT(&me->tmr, &e_tmr, NULL);
 }
 /* ---------------------------- Effect actions ----------------------------- */
 static void sendMotion(PlatC *const me, RKH_EVT_T *pe)
 {
	if(g_controlData.isConnected)
	{
		struct platC_packet packet;

		packet.values = bsp_getMotionData();
		packet.time   = bsp_getTimeSec();

		bsp_sendBleData((const rui8_t *) formatPlatCData(&packet, stPress));
	}
 }
 static void sendMotionByButton(PlatC *const me, RKH_EVT_T *pe)
 {
	sendMotion(me, pe);
 }
 static void sendMotionByCmd(PlatC *const me, RKH_EVT_T *pe)
 {
	sendMotion(me, pe);
 }
 static void updateCfg(PlatC *const me, RKH_EVT_T *pe)
 {
     evt_config_t *p = (evt_config_t *)(pe);

	 /* Check values sanity */
	 if( ((MOTION_THRESHOLD_MIN <= p->data.motionThr) && (p->data.motionThr <= MOTION_THRESHOLD_MAX)) &&
		 ((0 == p->data.buzzerEn) || (1 == p->data.buzzerEn)) )
	 {
		 /* Set PlatC eeprom data */
		 if(!bsp_writeEepromData(EMU_EEPROM_MOTION_THRESHOLD, (rui8_t *) &p->data.motionThr))
			RKH_ERROR();

		 if(!bsp_writeEepromData(EMU_EEPROM_ADVERTISE_NAME, (rui8_t *) &p->data.advName))
			RKH_ERROR();

		 if(!bsp_writeEepromData(EMU_EEPROM_BUZZER_ENABLED, (rui8_t *) &p->data.buzzerEn))
			RKH_ERROR();

		/* Set PlatC RAM data */
		g_controlData.motionThreshold = p->data.motionThr;
		memcpy(g_controlData.advName, p->data.advName, min(sizeof(g_controlData.advName), sizeof(p->data.advName)));
		g_controlData.isBuzzerEnabled = p->data.buzzerEn;
	 }
     
     bsp_sendBleData((const rui8_t *) formatPlatCAck());
 }
 static void sendCfg(PlatC *const me, RKH_EVT_T *pe)
 {
	bsp_sendBleData((const rui8_t *) formatPlatCCfg());
 }
 static void updateFitness(PlatC *const me, RKH_EVT_T *pe)
 {
     evt_fitness_t *p = (evt_fitness_t *)(pe);

	 /* Update fitness flag */
	 g_controlData.isFitnessRunning = p->data.fitnessFlag;

	 bsp_sendBleData((const rui8_t *) formatPlatCAck());
 }
 static void setBatteryChargingStatus(PlatC *const me, RKH_EVT_T *pe)
 {
	/* Set battery charging status */
	g_controlData.isBatteryCharging = true;

	/* Set leds status */
	setLedsStatus();
 }
 static void unsetBatteryChargingStatus(PlatC *const me, RKH_EVT_T *pe)
 {
	/* Set battery charging status */
	g_controlData.isBatteryCharging = false;

	/* Set leds status */
	setLedsStatus();
 }
 /* ----------------------------- Entry actions ----------------------------- */
 static void linkedEntry(PlatC *const me)
 {
	/* Set ble connected status */
	g_controlData.isConnected = true;

	/* Set leds status */
	setLedsStatus();
 }
 static void unlinkedEntry(PlatC *const me)
 {
 	 /* Set ble connected status */
 	 g_controlData.isConnected = false;

	 /* Set fitness running flag */
	 g_controlData.isFitnessRunning = false;

	 /* Set leds status */
	 setLedsStatus();

	 /* Start bluetooth module advertising */
	 bsp_startBleAdvertisement();

	 /* Set advertising flag */
	 g_controlData.isAdvertising = true;

	 /* Set static event */
	 RKH_SET_STATIC_EVENT(&e_tmr, evUnlikedTout);

	 /* Set one shot timer of UNLINKED_TIMEOUT seconds */
	 RKH_TMR_ONESHOT(&me->tmr, platC, UNLINKED_TIMEOUT);
 }
 static void hiddenEntry(PlatC *const me)
 {
 	/* Maintain bluetooth module reset it */
 	bsp_stopBleAdvertisement();

	/* Set advertising flag */
	g_controlData.isAdvertising = false;

	/* Stay in STANDBY sleep until low voltage is detected */
	// system_sleep();
 }
 static void motionEntry(PlatC *const me)
 {
	/* Set event motion indicator timeout */
	RKH_SET_STATIC_EVENT(&e_tmr, evMotionIndicatorTout);
	
	/* Set one shot timer of MOTION_DET_TIME seconds */
	RKH_TMR_ONESHOT(&me->tmr, platC, MOTION_DET_TIME);

	/* Set motion detect status */
	g_controlData.isMotionDetect = true;

	/* Set leds status */
	setLedsStatus();

	/* Set buzzer */
	setBuzzer();
 }
 /* ----------------------------- Exit actions ------------------------------ */
 static void linkedExit(PlatC *const me)
 {
	// Nothing to do here..
 }
 static void unlinkedExit(PlatC *const me)
 {
	 rkh_tmr_stop(&me->tmr);
 }
 static void hiddenExit(PlatC *const me)
 {
	// Nothing to do here..
 }
 static void motionExit(PlatC *const me)
 {
    rkh_tmr_stop(&me->tmr);

	/* Set motion detect status */
	g_controlData.isMotionDetect = false;

	/* Set leds status */
	setLedsStatus();

	/* Unset buzzer */
	unsetBuzzer();
 }
 /* -------------------------------- Guards --------------------------------- */
 /* ---------------------------- Global functions --------------------------- */
 /* ------------------------------ End of file ------------------------------ */