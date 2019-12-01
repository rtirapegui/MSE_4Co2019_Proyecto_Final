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
 *  \file       mpu9250.c
 *  \brief      MPU 9250 Sensor Support Driver for PlatC.
 *
 *  \ingroup    bsp
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2019.06.30  RoTi  v1.0.00  Initial version
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  RoTi  Rodrigo Tirapegui    rtirapeg@itba.edu.ar
 */
 /* --------------------------------- Notes --------------------------------- */
 /* ----------------------------- Include files ----------------------------- */
 #include "conf_sercoms.h"

 #include "mpu9250.h"
 #include "mpu9250_regs.h"
 #include "i2c_master.h"
 #include "extint.h"
 #include "extint_callback.h"
 #include "delay.h"
 #include "platCbsp.h"
 
 /* ----------------------------- Local macros ------------------------------ */
 /* ------------------------------- Constants ------------------------------- */
 #define I2C_READ_DELAY_MS					1
 #define I2C_WRITE_DELAY_MS					10
 #define LONG_WAIT_DELAY_MS					100
 #define MPU_9250_WHO_I_AM_EXPECTED_VALUE_1	0x71
 #define MPU_9250_WHO_I_AM_EXPECTED_VALUE_2	0x73
 #define AK8963_WHO_I_AM_EXPECTED_VALUE_1	0x48
 #define AK8963_WHO_I_AM_EXPECTED_VALUE_2	0x00

 /* ---------------------------- Local data types --------------------------- */
 /* Control structure for MPU9250 operation (only one IMU per project) */
 typedef struct
 {
	 MPU9250_address_t address; // MPU9250 address can be configured through AD0 pin
	 
	 // Scale factors
	 float _accelScale;
	 float _gyroScale;
	 float _magScaleX;
	 float _magScaleY;
	 float _magScaleZ;
	 float _tempScale;
	 float _tempOffset;
	 
	 // Configuration
	 MPU9250_AccelRange_t    _accelRange;
	 MPU9250_GyroRange_t     _gyroRange;
	 MPU9250_DlpfBandwidth_t _bandwidth;
	 rui8_t _srd;

	 // Buffer for reading from sensor
	 rui8_t _buffer[21];

	 // Data buffer
	 float _ax, _ay, _az;
	 float _gx, _gy, _gz;
	 float _hx, _hy, _hz;
	 float _t;

	 // Gyro bias estimation
	 rui8_t _numSamples;
	 double _gxbD, _gybD, _gzbD;
	 float _gxb, _gyb, _gzb;

	 // Accel bias and scale factor estimation
	 double _axbD, _aybD, _azbD;
	 float _axmax, _aymax, _azmax;
	 float _axmin, _aymin, _azmin;
	 float _axb, _ayb, _azb;
	 float _axs;
	 float _ays;
	 float _azs;

	 // Magnetometer bias and scale factor estimation
	 rui16_t _maxCounts;
	 float _deltaThresh;
	 rui8_t _coeff;
	 rui16_t _counter;
	 float _framedelta, _delta;
	 float _hxfilt, _hyfilt, _hzfilt;
	 float _hxmax, _hymax, _hzmax;
	 float _hxmin, _hymin, _hzmin;
	 float _hxb, _hyb, _hzb;
	 float _hxs;
	 float _hys;
	 float _hzs;
	 float _avgs;

	 // Data counts
	 ri16_t _axcounts, _aycounts, _azcounts;
	 ri16_t _gxcounts, _gycounts, _gzcounts;
	 ri16_t _hxcounts, _hycounts, _hzcounts;
	 ri16_t _tcounts;

	 // Transformation matrix
	 /* Transform the accel and gyro axes to match the magnetometer axes */
	 ri16_t tX[3];
	 ri16_t tY[3];
	 ri16_t tZ[3];

 } MPU9250_control_t;

 /* ---------------------------- Global variables --------------------------- */
 /* ---------------------------- Local variables ---------------------------- */
 
 /* User IRQ callback */
 static mpu9250_IRQCb g_userCb;

 /* MPU i2c module handler */
 static struct i2c_master_module g_i2cHandler;
 
 /* MPU g_control structure */
 static MPU9250_control_t g_control = 
 {
	.address = MPU9250_ADDRESS_1
 };

 /* ----------------------- Local function prototypes ----------------------- */
 /* ---------------------------- Local functions ---------------------------- */
 static void mpu9250InitializeControlStructure(void)
 {
	 g_control._tempScale = 333.87f;
	 g_control._tempOffset = 21.0f;
	 g_control._numSamples = 100;
	 g_control._axs = 1.0f;
	 g_control._ays = 1.0f;
	 g_control._azs = 1.0f;
	 g_control._maxCounts = 1000;
	 g_control._deltaThresh = 0.3f;
	 g_control._coeff = 8;
	 g_control._hxs = 1.0f;
	 g_control._hys = 1.0f;
	 g_control._hzs = 1.0f;
	 g_control.tX[0] = -1;
	 g_control.tX[1] = 0;
	 g_control.tX[2] = 0;
	 g_control.tY[0] = 0;
	 g_control.tY[1] = -1;
	 g_control.tY[2] = 0;
	 g_control.tZ[0] = 0;
	 g_control.tZ[1] = 0;
	 g_control.tZ[2] = -1;
 }
 static rbool_t mpu9250ReadRegister(rui8_t subAddress, rui8_t count)
 {
	 struct i2c_master_packet packet =
	 {
		 .address         = g_control.address,
		 .data_length     = 1,
		 .data			  = &subAddress,
		 .ten_bit_address = false,
		 .high_speed      = false,
		 .hs_master_code  = 0x0,
	 };

	 /* Write buffer to slave */
	 if(STATUS_OK != i2c_master_write_packet_wait(&g_i2cHandler, &packet))
		return false;

	 packet.data_length = min(count, sizeof(g_control._buffer));
	 packet.data = g_control._buffer;

	 /* Read buffer from slave */
	 if(STATUS_OK != i2c_master_read_packet_wait(&g_i2cHandler, &packet))
		return false;

	 return true;
 }
 static rbool_t mpu9250WriteRegister(rui8_t subAddress, rui8_t data)
 {
	rui8_t txBuffer[2];

	txBuffer[0] = subAddress;
	txBuffer[1] = data;

	struct i2c_master_packet packet = 
	{
		.address         = g_control.address,
		.data_length     = sizeof(txBuffer),
		.data			 = txBuffer,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	/* Write buffer to slave */
	if(STATUS_OK != i2c_master_write_packet_wait(&g_i2cHandler, &packet))
		return false;

	delay_ms(I2C_WRITE_DELAY_MS);

	/* Read back the register */
	mpu9250ReadRegister(subAddress,1);
	
	/* Check the read back register against the written register */
	if(g_control._buffer[0] != data) 
		return false;

    return true;
 }
 static rbool_t mpu9250ReadAK8963Registers(rui8_t subAddress, rui8_t count)
{
	/* Set slave 0 to the AK8963 and set for read */
	if(!mpu9250WriteRegister(MPU9250_I2C_SLV0_ADDR, MPU9250_AK8963_I2C_ADDR | MPU9250_I2C_READ_FLAG))
		return false;

	/* Set the register to the desired AK8963 sub address */
	if(!mpu9250WriteRegister(MPU9250_I2C_SLV0_REG, subAddress))
		return false;

	/* Enable I2C and request the bytes */
	if(!mpu9250WriteRegister(MPU9250_I2C_SLV0_CTRL, MPU9250_I2C_SLV0_EN | count))
		return false;

	/* Takes some time for these registers to fill */
	delay_ms(I2C_READ_DELAY_MS);
	
	/* Read the bytes off the MPU9250 EXT_SENS_DATA registers */
	return mpu9250ReadRegister(MPU9250_EXT_SENS_DATA_00, count);
}
 static rbool_t mpu9250WriteAK8963Register(rui8_t subAddress, rui8_t data)
{
	/* Set slave 0 to the AK8963 and set for write */
	if(!mpu9250WriteRegister(MPU9250_I2C_SLV0_ADDR, MPU9250_AK8963_I2C_ADDR))
		return false;

	/* Set the register to the desired AK8963 sub address */
	if(!mpu9250WriteRegister(MPU9250_I2C_SLV0_REG, subAddress))
		return false;

	/* Store the data for write */
	if(!mpu9250WriteRegister(MPU9250_I2C_SLV0_DO, data))
		return false;

	/* Enable I2C and send 1 byte */
	if(!mpu9250WriteRegister(MPU9250_I2C_SLV0_CTRL, MPU9250_I2C_SLV0_EN | (rui8_t)1))
		return false;

	/* Read the register and confirm */
	if(!mpu9250ReadAK8963Registers(subAddress, 1))
		return false;

	if(data != g_control._buffer[0])
		return false;

	return true;
}
 static rbool_t mpu9250WhoAmI(rui8_t *val)
{
	/* Read the WHO AM I register */
	if(!mpu9250ReadRegister(MPU9250_WHO_AM_I, 1))
		return false;

	/* Return the register value */
	if(val)
		*val = g_control._buffer[0];

	return true;
}
 static rbool_t mpu9250SetGyroRange(MPU9250_GyroRange_t range)
{
	switch(range) 
	{
		case MPU9250_GYRO_RANGE_250DPS: 
		{
			// Setting the gyro range to 250DPS
			if(!mpu9250WriteRegister(MPU9250_GYRO_CONFIG, MPU9250_GYRO_FS_SEL_250DPS))
				return false;

			// Setting the gyro scale to 250DPS
			g_control._gyroScale = 250.0f/32767.5f * MPU9250_D2R;
			break;
		}
		case MPU9250_GYRO_RANGE_500DPS: 
		{
			// Setting the gyro range to 500DPS
			if(!mpu9250WriteRegister(MPU9250_GYRO_CONFIG, MPU9250_GYRO_FS_SEL_500DPS))
				return false;

			// Setting the gyro scale to 500DPS
			g_control._gyroScale = 500.0f/32767.5f * MPU9250_D2R;
			break;
		}
		case MPU9250_GYRO_RANGE_1000DPS: 
		{
			// Setting the gyro range to 1000DPS
			if(!mpu9250WriteRegister(MPU9250_GYRO_CONFIG, MPU9250_GYRO_FS_SEL_1000DPS))
				return false;

			// Setting the gyro scale to 1000DPS
			g_control._gyroScale = 1000.0f/32767.5f * MPU9250_D2R;
			break;
		}
		case MPU9250_GYRO_RANGE_2000DPS: 
		{
			// Setting the gyro range to 2000DPS
			if(!mpu9250WriteRegister(MPU9250_GYRO_CONFIG, MPU9250_GYRO_FS_SEL_2000DPS))
				return false;

			// Setting the gyro scale to 2000DPS
			g_control._gyroScale = 2000.0f/32767.5f * MPU9250_D2R;
			break;
		}
	}
	
	g_control._gyroRange = range;
	
	return true;
}
 static rbool_t mpu9250SetDlpfBandwidth(MPU9250_DlpfBandwidth_t bandwidth)
{
	switch (bandwidth) 
	{
		case MPU9250_DLPF_BANDWIDTH_184HZ: 
		{
			// Setting accel bandwidth to 184Hz
			if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG2, MPU9250_ACCEL_DLPF_184))
				return false;
			
			// Setting gyro bandwidth to 184Hz
			if(!mpu9250WriteRegister(MPU9250_CONFIG, MPU9250_GYRO_DLPF_184))
				return false;

			break;
		}
		case MPU9250_DLPF_BANDWIDTH_92HZ: 
		{
			// Setting accel bandwidth to 92Hz
			if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG2, MPU9250_ACCEL_DLPF_92))
				return false; 

			// Setting gyro bandwidth to 92Hz
			if(!mpu9250WriteRegister(MPU9250_CONFIG, MPU9250_GYRO_DLPF_92))
				return false; 

			break;
		}
		case MPU9250_DLPF_BANDWIDTH_41HZ: 
		{
			// Setting accel bandwidth to 41Hz
			if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG2, MPU9250_ACCEL_DLPF_41))
				return false;

			// Setting gyro bandwidth to 41Hz
			if(!mpu9250WriteRegister(MPU9250_CONFIG, MPU9250_GYRO_DLPF_41))
				return false;

			break;
		}
		case MPU9250_DLPF_BANDWIDTH_20HZ: 
		{
			// Setting accel bandwidth to 20Hz
			if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG2, MPU9250_ACCEL_DLPF_20))
				return false; 

			// Setting gyro bandwidth to 20Hz
			if(!mpu9250WriteRegister(MPU9250_CONFIG, MPU9250_GYRO_DLPF_20))
				return false; 

			break;
		}
		case MPU9250_DLPF_BANDWIDTH_10HZ: 
		{
			// Setting accel bandwidth to 10Hz
			if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG2, MPU9250_ACCEL_DLPF_10))
				return false;

			// Setting gyro bandwidth to 10Hz
			if(!mpu9250WriteRegister(MPU9250_CONFIG, MPU9250_GYRO_DLPF_10))
				return false;

			break;
		}
		case MPU9250_DLPF_BANDWIDTH_5HZ: 
		{
			// Setting accel bandwidth to 5Hz
			if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG2, MPU9250_ACCEL_DLPF_5))
				return false; 

			// Setting gyro bandwidth to 5Hz
			if(!mpu9250WriteRegister(MPU9250_CONFIG, MPU9250_GYRO_DLPF_5))
				return false; 

			break;
		}
	}
	
	g_control._bandwidth = bandwidth;
	return true;
}
 static rbool_t mpu9250Read(void)
{
	/* Grab the data from the MPU9250 */
	if(!mpu9250ReadRegister(MPU9250_ACCEL_OUT, 21))
		return false;
	
	/* Combine into 16 bit values */
	g_control._axcounts = (((ri16_t)g_control._buffer[0]) << 8)  | g_control._buffer[1];
	g_control._aycounts = (((ri16_t)g_control._buffer[2]) << 8)  | g_control._buffer[3];
	g_control._azcounts = (((ri16_t)g_control._buffer[4]) << 8)  | g_control._buffer[5];
	g_control._tcounts  = (((ri16_t)g_control._buffer[6]) << 8)  | g_control._buffer[7];
	g_control._gxcounts = (((ri16_t)g_control._buffer[8]) << 8)  | g_control._buffer[9];
	g_control._gycounts = (((ri16_t)g_control._buffer[10]) << 8) | g_control._buffer[11];
	g_control._gzcounts = (((ri16_t)g_control._buffer[12]) << 8) | g_control._buffer[13];
	g_control._hxcounts = (((ri16_t)g_control._buffer[15]) << 8) | g_control._buffer[14];
	g_control._hycounts = (((ri16_t)g_control._buffer[17]) << 8) | g_control._buffer[16];
	g_control._hzcounts = (((ri16_t)g_control._buffer[19]) << 8) | g_control._buffer[18];
	
	/* Transform and convert to float values */
	g_control._ax = (((float)(g_control.tX[0]*g_control._axcounts + g_control.tX[1]*g_control._aycounts + g_control.tX[2]*g_control._azcounts) * g_control._accelScale) - g_control._axb)*g_control._axs;
	g_control._ay = (((float)(g_control.tY[0]*g_control._axcounts + g_control.tY[1]*g_control._aycounts + g_control.tY[2]*g_control._azcounts) * g_control._accelScale) - g_control._ayb)*g_control._ays;
	g_control._az = (((float)(g_control.tZ[0]*g_control._axcounts + g_control.tZ[1]*g_control._aycounts + g_control.tZ[2]*g_control._azcounts) * g_control._accelScale) - g_control._azb)*g_control._azs;
	g_control._gx = ((float) (g_control.tX[0]*g_control._gxcounts + g_control.tX[1]*g_control._gycounts + g_control.tX[2]*g_control._gzcounts) * g_control._gyroScale) -  g_control._gxb;
	g_control._gy = ((float) (g_control.tY[0]*g_control._gxcounts + g_control.tY[1]*g_control._gycounts + g_control.tY[2]*g_control._gzcounts) * g_control._gyroScale) -  g_control._gyb;
	g_control._gz = ((float) (g_control.tZ[0]*g_control._gxcounts + g_control.tZ[1]*g_control._gycounts + g_control.tZ[2]*g_control._gzcounts) * g_control._gyroScale) -  g_control._gzb;
	g_control._hx = (((float)(g_control._hxcounts) * g_control._magScaleX) - g_control._hxb)*g_control._hxs;
	g_control._hy = (((float)(g_control._hycounts) * g_control._magScaleY) - g_control._hyb)*g_control._hys;
	g_control._hz = (((float)(g_control._hzcounts) * g_control._magScaleZ) - g_control._hzb)*g_control._hzs;
	g_control._t  = ((((float) g_control._tcounts)  - g_control._tempOffset)/ g_control._tempScale) + g_control._tempOffset;
	
	return true;
}
 static rbool_t mpu9250CalibrateGyro(void)
{
	/* Set the range, bandwidth, and srd */
	if(!mpu9250SetGyroRange(MPU9250_GYRO_RANGE_250DPS))
		return false;

	if(!mpu9250SetDlpfBandwidth(MPU9250_DLPF_BANDWIDTH_20HZ))
		return false;

	/* Take samples and find bias */
	g_control._gxbD = 0;
	g_control._gybD = 0;
	g_control._gzbD = 0;
	
	for (rui8_t i = 0; i < g_control._numSamples; i++) 
	{
		mpu9250Read();
		g_control._gxbD += ((g_control._gx + g_control._gxb)/g_control._numSamples);
		g_control._gybD += ((g_control._gy + g_control._gyb)/g_control._numSamples);
		g_control._gzbD += ((g_control._gz + g_control._gzb)/g_control._numSamples);
		delay_ms(LONG_WAIT_DELAY_MS);
	}

	g_control._gxb = (float) g_control._gxbD;
	g_control._gyb = (float) g_control._gybD;
	g_control._gzb = (float) g_control._gzbD;

	/* Set the range, bandwidth, and srd back to what they were */
	if(!mpu9250SetGyroRange(g_control._gyroRange))
		return false;

	if(!mpu9250SetDlpfBandwidth(g_control._bandwidth))
		return false;

	return true;
}
 static rbool_t mpu9250Begin(void)
 {
    rui8_t val;

	/* Initialize g_control structure */
	mpu9250InitializeControlStructure();

	/* Select clock source to gyro */
	if(!mpu9250WriteRegister(MPU9250_PWR_MGMNT_1, MPU9250_CLOCK_SEL_PLL))
		return false;
 
	/* Enable I2C master mode */
	if(!mpu9250WriteRegister(MPU9250_USER_CTRL, MPU9250_I2C_MST_EN))
		return false;

	/* Set the I2C bus speed to 400 kHz */
	if(!mpu9250WriteRegister(MPU9250_I2C_MST_CTRL, MPU9250_I2C_MST_CLK))
		return false;

	/* Set AK8963 to Power Down */
	if(!mpu9250WriteAK8963Register(MPU9250_AK8963_CNTL1, MPU9250_AK8963_PWR_DOWN))
		return false;
 
	/* Check the WHO AM I byte, expected value is 0x71 (decimal 113) or 0x73 (decimal 115) */
	if(!mpu9250WhoAmI(&val))
		return false;

	if((MPU_9250_WHO_I_AM_EXPECTED_VALUE_1 != val) && (MPU_9250_WHO_I_AM_EXPECTED_VALUE_2 != val))
		return false;

	/* Enable accelerometer and gyro */
	if(!mpu9250WriteRegister(MPU9250_PWR_MGMNT_2, MPU9250_SEN_ENABLE))
		return false;

	/* Setting accel range to 2G as default */
	if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG, MPU9250_ACCEL_FS_SEL_2G))
		return false;

	/* Setting the accel scale to 2 */
	g_control._accelScale = MPU9250_G * 2.0f / 32767.5f;
	g_control._accelRange = MPU9250_ACCEL_RANGE_2G;
 
	/* Setting the gyro range to 250DPS as default */
	if(!mpu9250WriteRegister(MPU9250_GYRO_CONFIG, MPU9250_GYRO_FS_SEL_250DPS))
		return false;
 
	/* Setting the gyro scale to 250DPS */
	g_control._gyroScale = 250.0f / 32767.5f * MPU9250_D2R;
	g_control._gyroRange = MPU9250_GYRO_RANGE_250DPS;
 
	/* Setting bandwidth to 184Hz as default */
	if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG2, MPU9250_ACCEL_DLPF_184))
		return false;

	/* Setting gyro bandwidth to 184Hz */
	if(!mpu9250WriteRegister(MPU9250_CONFIG, MPU9250_GYRO_DLPF_184))
		return false;

	g_control._bandwidth = MPU9250_DLPF_BANDWIDTH_184HZ;
 
	/* Setting the sample rate divider to 0 as default */
	if(!mpu9250WriteRegister(MPU9250_SMPDIV, 0x00))
		return false;

	g_control._srd = 0;
 
	/* Set the I2C bus speed to 400 kHz */
	if(!mpu9250WriteRegister(MPU9250_I2C_MST_CTRL, MPU9250_I2C_MST_CLK))
		return false;

	/* Estimate gyro bias */
	if(!mpu9250CalibrateGyro())
		return false;

	return true;
 }
 static rbool_t mpu9250EnableWakeOnMotion(rui16_t womThreshold)
 {
	 /* Set AK8963 to Power Down */
	 mpu9250WriteAK8963Register(MPU9250_AK8963_CNTL1, MPU9250_AK8963_PWR_DOWN);
	 
	 /* Set cycle 0, sleep 0, standby 0 */
	 if(!mpu9250WriteRegister(MPU9250_PWR_MGMNT_1, 0x00))
		return false;

	 /* Disable gyro measurements */
	 if(!mpu9250WriteRegister(MPU9250_PWR_MGMNT_2, MPU9250_DIS_GYRO_EN_ACCEL_Y))
		return false;
	 
	 /* Setting accel bandwidth to 184Hz */
	 if(!mpu9250WriteRegister(MPU9250_ACCEL_CONFIG2, MPU9250_ACCEL_DLPF_184))
		return false;

	 /* Enabling interrupt to wake on motion */
	 if(!mpu9250WriteRegister(MPU9250_INT_ENABLE, MPU9250_INT_WOM_EN))
		return false;

	 /* Enabling accel hardware intelligence */
	 if(!mpu9250WriteRegister(MPU9250_MOT_DETECT_CTRL, (MPU9250_ACCEL_INTEL_EN | MPU9250_ACCEL_INTEL_MODE)))
		return false;

	 /* Get Wake On Motion threshold */
	 womThreshold *= 255;
	 womThreshold /= 1020;

	 /* Setting wake on motion threshold */
	 if(!mpu9250WriteRegister(MPU9250_WOM_THR, womThreshold))
		return false;

	 /* Set frequency of wakeup */
	 if(!mpu9250WriteRegister(MPU9250_LP_ACCEL_ODR, (rui8_t) MPU9250_LP_ACCEL_ODR_15_63HZ))
		return false;
#if 0	// @todo Not necessary by now
     /* Switch to accel low power mode */
	 if(!mpu9250WriteRegister(MPU9250_PWR_MGMNT_1, MPU9250_PWR_CYCLE))
		return false;
#endif

	 return true;
 }
 static void EXT_WAKE_UP_IRQCb(void)
 {
 	MPU9250_data_t userData = { 0 };

	if(g_userCb)
	{
		if(!mpu9250Read())
			return;
		
		userData.ax = g_control._ax;
		userData.ay = g_control._ay;
		userData.az = g_control._az;
		userData.gx = g_control._gx;
		userData.gy = g_control._gy;
		userData.gz = g_control._gz;
		userData.hx = g_control._hx;
		userData.hy = g_control._hy;
		userData.hz = g_control._hz;
		userData.t  = g_control._t;

		/* Invoke user callback */
		(g_userCb) (userData);
	}
 }
 
 /* ---------------------------- Global functions --------------------------- */
 rbool_t mpu9250_init(void)
 {
	 struct i2c_master_config i2c_config;
	 struct extint_chan_conf extint_config;

	 /* Set default configuration */
	 i2c_master_get_config_defaults(&i2c_config);

	 i2c_config.baud_rate = MPU9250_I2C_RATE;
	 i2c_config.buffer_timeout = 10000;

	 /* Initialize i2c as master */
	 i2c_master_init(&g_i2cHandler, MPU9250_I2C_MASTER_MODULE, &i2c_config);

	 /* Enable i2c module */
	 i2c_master_enable(&g_i2cHandler);

	 /* Set basic configuration */
	if(!mpu9250Begin())
		return false;

	 /* Configure external interrupt */
	 extint_chan_get_config_defaults(&extint_config);

	 extint_config.gpio_pin            = EXT_WAKE_UP_IRQ_PIN;
	 extint_config.gpio_pin_mux        = EXT_WAKE_UP_IRQ_MUX;
	 extint_config.gpio_pin_pull       = EXTINT_PULL_NONE;
	 extint_config.detection_criteria  = EXTINT_DETECT_FALLING;
	 extint_config.filter_input_signal = true;

	 extint_chan_set_config(EXT_WAKE_UP_IRQ_LINE, &extint_config);

	 /* Configure external interrupt callback */
	 extint_register_callback(EXT_WAKE_UP_IRQCb, EXT_WAKE_UP_IRQ_LINE, EXTINT_CALLBACK_TYPE_DETECT);

	 /* Enable external interrupt callback */
	 extint_chan_enable_callback(EXT_WAKE_UP_IRQ_LINE, EXTINT_CALLBACK_TYPE_DETECT);

	 return true;
 }
 MPU9250_data_t mpu9250_getData(void)
 {
	MPU9250_data_t userData = { 0 };

	if(mpu9250Read())
	{
		userData.ax = g_control._ax;
		userData.ay = g_control._ay;
		userData.az = g_control._az;
		userData.gx = g_control._gx;
		userData.gy = g_control._gy;
		userData.gz = g_control._gz;
		userData.hx = g_control._hx;
		userData.hy = g_control._hy;
		userData.hz = g_control._hz;
		userData.t  = g_control._t;
	}

	return userData;
 }
 rbool_t mpu9250_setMotionThreshold(rui16_t womTheshold)
 {
	/* Enable Wake On Motion mode */
	return mpu9250EnableWakeOnMotion(womTheshold);
 }
 void mpu9250_registerCb(mpu9250_IRQCb cb)
 {
	if(cb)
	{
		RKH_ENTER_CRITICAL();
		g_userCb = cb;
		RKH_EXIT_CRITICAL();
	}
 }
  
 /* ------------------------------ End of file ------------------------------ */

