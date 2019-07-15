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
 #include "conf_app.h"

 #include "bsp.h"
 #include "platC.h"

 /* ----------------------------- Local macros ------------------------------ */
 #define UNLINKED_TIMEOUT        RKH_TIME_SEC(60)
 #define MOTION_DET_TIME         RKH_TIME_MS(3000)
 #define KEEP_ALIVE_TIME		 RKH_TIME_MS(500)
 #define SHORT2LONG_BUTT_TIME    RKH_TIME_MS(3000)
 #define DEBOUNCING_SAMPLES      7
 #define PRESS_MASK              (1 << DEBOUNCING_SAMPLES)
 #define RELEASE_MASK            (~(PRESS_MASK) && 0xFF)

 typedef enum
 {
	 stReleased,
	 stPress,
	 stLongPress
 } push_button_state_t;

 /* ------------------------- Declares active object ------------------------ */
 typedef struct PlatC PlatC;
 typedef struct evt_motion evt_motion_t;
 typedef struct platC_packet platC_packet_t;
 typedef struct platC_control platC_control_t;
 /* ------------------- Declares states and pseudostates -------------------- */
 RKH_DCLR_COMP_STATE   steady;
 RKH_DCLR_BASIC_STATE  unlinked, linked, hidden, motionDetect;
 RKH_DCLR_SHIST_STATE  steadySHist;

 /* ------------------------ Declares initial action ------------------------ */
  static void init(PlatC *const me, RKH_EVT_T *pe);
 /* ------------------------ Declares effect actions ------------------------ */
  static void sendMotionByIRQ(PlatC *const me, RKH_EVT_T *pe);
  static void sendMotionByButton(PlatC *const me, RKH_EVT_T *pe);
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
	 //RKH_TRINT(evPlatCCfg, NULL, &updateCfg),
	 //RKH_TRINT(evSporaGetCfg, NULL, &sendCfg),
	 RKH_TRINT(evBatteryChargerConnected, NULL, &setBatteryChargingStatus),
	 RKH_TRINT(evBatteryChargerDisconnected, NULL, &unsetBatteryChargingStatus),
	 RKH_TRREG(evMotionDetect, NULL, NULL, &motionDetect),
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
	 //RKH_TRINT(evPlatCGetData, NULL, &sendData),
	 RKH_TRINT(evBatteryChargerConnected, NULL, &setBatteryChargingStatus),
	 RKH_TRINT(evBatteryChargerDisconnected, NULL, &unsetBatteryChargingStatus),
	 RKH_TRREG(evMotionDetect, NULL, sendMotionByIRQ, &motionDetect),
	 RKH_TRREG(evDisconnected, NULL, NULL, &unlinked),
	 RKH_TRREG(evPushbuttonLongPress, NULL, NULL, &unlinked),
	 RKH_TRREG(evPushbuttonShortPress, NULL, sendMotionByButton, &motionDetect),
 RKH_END_TRANS_TABLE

 RKH_CREATE_BASIC_STATE(motionDetect, motionEntry, motionExit, RKH_ROOT, NULL);
 RKH_CREATE_TRANS_TABLE(motionDetect)
	 //RKH_TRINT(evPlatCCfg, NULL, &updateCfg),
	 //RKH_TRINT(evPlatCGetData, NULL, &sendCfg),
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

 /* ---------------------------- Local Variables ---------------------------- */
 #if defined(RKH_USE_TRC_SENDER)
	static rui8_t pushbutton;
	static rui8_t ble;
	static rui8_t accel
	static rui8_t battery;
 #endif

 static RKH_STATIC_EVENT(e_tmr, evInitTout);
 static RKH_STATIC_EVENT(e_pushbutton, evPushbuttonLongPress);
 static RKH_STATIC_EVENT(e_ble, evOk);
 static RKH_STATIC_EVENT(e_battery, evBatteryChargerConnected);
 
 static evt_motion_t g_motionData;
 static platC_control_t g_controlData;
 /* ---------------------------- Local data types --------------------------- */
struct platC_packet
 {
	MPU9250_data_t values;
	rui32_t time;
 };
 
 struct evt_motion
 {
	 RKH_EVT_T e;
	 struct platC_packet data;
 };

 struct platC_control
 {
	rbool_t isConnected;
	rbool_t isMotionDetect;
	rbool_t isBatteryCharging;
	rbool_t isBuzzerEnabled;
 };
 /* ----------------------- Local function prototypes ----------------------- */
 /* ---------------------------- Local functions ---------------------------- */
 static void BATTERY_IRQ(rbool_t isCharging)
 {
	if(isCharging)
	{
		/* Set event battery charger connected */
		RKH_SET_STATIC_EVENT(&e_battery, evBatteryChargerConnected);
	}
	else
	{
		/* Set event battery charger disconnected */
		RKH_SET_STATIC_EVENT(&e_battery, evBatteryChargerDisconnected);
	}

	/* Post battery event to platC object */
	RKH_SMA_POST_FIFO(platC, &e_battery, &battery);
 }
 static void BLE_IRQ(rbool_t isConnected)
 {
	 if(isConnected)
	 {
		 /* Set event ble connected */
		 RKH_SET_STATIC_EVENT(&e_ble, evConnected);
	 }
	 else
	 {
		 /* Set event ble disconnected */
		 RKH_SET_STATIC_EVENT(&e_ble, evDisconnected);
	 }

	 /* Post push button event to platC object */
	 RKH_SMA_POST_FIFO(platC, &e_ble, &ble);
 }
 static void ACCEL_IRQ(MPU9250_data_t data)
 {
	/* Set event evMotionDetect */
	RKH_SET_STATIC_EVENT(&g_motionData, evMotionDetect);

    /* 
     * For getting accelerometer latched values, first read 
     * output registers before interrupt acknowledge.
     */
    g_motionData.data.values = data;
	g_motionData.data.time   = bsp_getTimeSec();
    
	/* Post event to active object */
    RKH_SMA_POST_FIFO(platC, CE(&g_motionData), &accel);
 }
 static void push_button_SM(void)
 {
	 static rui32_t state, filter, button, tout;
 
	 /* Debouncer filter */
	 filter = (filter << 1) | PUSH_BUTTON_GET_STATUS();

	 if(filter == RELEASE_MASK)
	 {
		 button = stReleased;
	 }
	 else if(filter == PRESS_MASK)
	 {
		 button = stPress;
	 }

	 /* Pushbutton state machine */
	 switch(state)
	 {
		 case stReleased:
		 {
			 if(button == stPress)
			 {
				 state = stPress;
				 tout = SHORT2LONG_BUTT_TIME;
			 }
		 } break;

		 case stPress:
		 {
			 if(button == stReleased)
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
			if(button == stReleased)
				state = stReleased;
		 } break;

		 default:
		 break;
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
		BUZZER_On();
 }
 static void unsetBuzzer(void)
 {
	 /* Turn off buzzer once motion has ended */
	 BUZZER_Off();
 }

 void rkh_hook_timetick(void)
 {	/* Get in here every RKH_TICK_RATE_MS */
	static rui32_t ledIndex, keepAliveTime = KEEP_ALIVE_TIME; 
		
	/* Invoke push button state machine */
	push_button_SM();

	/* Toggle MOTION, BLE and BATTERY LED every KEEP_ALIVE_TIME */
	if(!g_controlData.isConnected && !g_controlData.isBatteryCharging && !g_controlData.isMotionDetect)
	{
		if(keepAliveTime && --keepAliveTime == 0)
		{
			keepAliveTime = KEEP_ALIVE_TIME;
			
			switch((ledIndex++)%3)
			{
				case 0:		LED_Toggle(MOTION_LED_PIN);		break;
				case 1:		LED_Toggle(BLE_LED_PIN);		break;
				case 2:		LED_Toggle(BATTERY_LED_PIN);	break;
				default:	ledIndex = 0;					break;
			}
		}
	}
 }
  
 /* ---------------------------- Initial action ----------------------------- */
 static void init(PlatC *const me, RKH_EVT_T *pe)
 {
	rui16_t motionThreshold;
	
	/* Register battery callback */
	bsp_registerBatteryCb(BATTERY_IRQ);

	/* Register accel module callback */
	bsp_registerAccelCb(ACCEL_IRQ);

	/* Register ble module callback */
	bsp_registerBleCb(BLE_IRQ);

	/* Load eeprom data */
	if(!bsp_readEepromData(EMU_EEPROM_MOTION_THRESHOLD, (rui8_t *) &motionThreshold))
		RKH_ERROR();

	if(!bsp_readEepromData(EMU_EEPROM_BUZZER_ENABLED, (rui8_t *) &g_controlData.isBuzzerEnabled))
		RKH_ERROR();

	/* Check eeprom data sanity */
	if(0xFFFF == motionThreshold)
		motionThreshold = MOTION_THRESHOLD_DEFAULT;

	if(0xFF == g_controlData.isBuzzerEnabled)
		g_controlData.isBuzzerEnabled = BUZZER_ENABLED_DEFAULT;

	/* Set motion threshold */
	if(!bsp_setMotionThreshold(motionThreshold))
		RKH_ERROR();

	RKH_TMR_INIT(&me->tmr, &e_tmr, NULL);
 }
 /* ---------------------------- Effect actions ----------------------------- */
 static void sendMotionByIRQ(PlatC *const me, RKH_EVT_T *pe)
 {
     //evt_motion_t *m = (evt_motion_t *)(pe);

     // codeless_sendData(formatSporaData(&m->data, RELEASE));
 }
 static void sendMotionByButton(PlatC *const me, RKH_EVT_T *pe)
 {
     //platC_packet_t packet;
     
     //packet.values = mpu9250_getData();
     //packet.time   = bsp_getTimeSec();

     // codeless_sendData(formatSporaData(&packet, PRESS));
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
	 /* Start bluetooth module advertising */
	 bsp_startBleAdvertisement();

	 /* Set static event */
	 RKH_SET_STATIC_EVENT(&e_tmr, evUnlikedTout);

	 /* Set one shot timer of UNLINKED_TIMEOUT seconds */
	 RKH_TMR_ONESHOT(&me->tmr, platC, UNLINKED_TIMEOUT);
 }
 static void hiddenEntry(PlatC *const me)
 {
 	/* Maintain bluetooth module reset it */
 	bsp_stopBleAdvertisement();
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
	/* Set ble connected status */
	g_controlData.isConnected = false;

	/* Set leds status */
	setLedsStatus();
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