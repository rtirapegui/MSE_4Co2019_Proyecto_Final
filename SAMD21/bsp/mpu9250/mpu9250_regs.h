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
 *  \file       mpu9250_regs.h
 *  \brief      MPU 9250 Sensor Support Driver Registers for PlatC.
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
#ifndef MPU9250_REGS_H_
#define MPU9250_REGS_H_

/* ----------------------------- Include files ----------------------------- */


/* ---------------------- External C language linkage ---------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- Macros -------------------------------- */
// Physical constants
#define MPU9250_G                     9.807f
#define MPU9250_D2R                   3.14159265359f/180.0f

// MPU9250 registers
#define MPU9250_ACCEL_OUT             0x3B
#define MPU9250_GYRO_OUT              0x43
#define MPU9250_TEMP_OUT              0x41
#define MPU9250_EXT_SENS_DATA_00      0x49
#define MPU9250_ACCEL_CONFIG 	      0x1C
#define MPU9250_ACCEL_FS_SEL_2G       0x00
#define MPU9250_ACCEL_FS_SEL_4G       0x08
#define MPU9250_ACCEL_FS_SEL_8G       0x10
#define MPU9250_ACCEL_FS_SEL_16G      0x18
#define MPU9250_GYRO_CONFIG           0x1B
#define MPU9250_GYRO_FS_SEL_250DPS    0x00
#define MPU9250_GYRO_FS_SEL_500DPS    0x08
#define MPU9250_GYRO_FS_SEL_1000DPS   0x10
#define MPU9250_GYRO_FS_SEL_2000DPS   0x18

#define MPU9250_ACCEL_CONFIG2         0x1D
#define MPU9250_ACCEL_DLPF_184        0x01
#define MPU9250_ACCEL_DLPF_92         0x02
#define MPU9250_ACCEL_DLPF_41         0x03
#define MPU9250_ACCEL_DLPF_20         0x04
#define MPU9250_ACCEL_DLPF_10         0x05
#define MPU9250_ACCEL_DLPF_5          0x06
#define MPU9250_CONFIG                0x1A
#define MPU9250_GYRO_DLPF_184         0x01
#define MPU9250_GYRO_DLPF_92          0x02
#define MPU9250_GYRO_DLPF_41          0x03
#define MPU9250_GYRO_DLPF_20          0x04
#define MPU9250_GYRO_DLPF_10          0x05
#define MPU9250_GYRO_DLPF_5           0x06
#define MPU9250_SMPDIV                0x19
#define MPU9250_INT_PIN_CFG           0x37
#define MPU9250_INT_ENABLE            0x38
#define MPU9250_INT_DISABLE           0x00
#define MPU9250_INT_PULSE_50US        0x00
#define MPU9250_INT_WOM_EN            0x40
#define MPU9250_INT_RAW_RDY_EN        0x01
#define MPU9250_PWR_MGMNT_1           0x6B
#define MPU9250_PWR_CYCLE             0x20
#define MPU9250_PWR_RESET             0x80
#define MPU9250_CLOCK_SEL_PLL         0x01
#define MPU9250_PWR_MGMNT_2           0x6C
#define MPU9250_SEN_ENABLE            0x00
#define MPU9250_DIS_GYRO              0x07
#define MPU9250_DIS_GYRO_EN_ACCEL_X   0x1F
#define MPU9250_DIS_GYRO_EN_ACCEL_Y   0x2F
#define MPU9250_DIS_GYRO_EN_ACCEL_Z   0x37
#define MPU9250_USER_CTRL             0x6A
#define MPU9250_I2C_MST_EN            0x20
#define MPU9250_I2C_MST_CLK           0x0D
#define MPU9250_I2C_MST_CTRL          0x24
#define MPU9250_I2C_SLV0_ADDR         0x25
#define MPU9250_I2C_SLV0_REG          0x26
#define MPU9250_I2C_SLV0_DO           0x63
#define MPU9250_I2C_SLV0_CTRL         0x27
#define MPU9250_I2C_SLV0_EN           0x80
#define MPU9250_I2C_READ_FLAG         0x80
#define MPU9250_MOT_DETECT_CTRL       0x69
#define MPU9250_ACCEL_INTEL_EN        0x80
#define MPU9250_ACCEL_INTEL_MODE      0x40
#define MPU9250_LP_ACCEL_ODR          0x1E
#define MPU9250_WOM_THR               0x1F
#define MPU9250_WHO_AM_I              0x75
#define MPU9250_FIFO_EN               0x23
#define MPU9250_FIFO_TEMP             0x80
#define MPU9250_FIFO_GYRO             0x70
#define MPU9250_FIFO_ACCEL            0x08
#define MPU9250_FIFO_MAG              0x01
#define MPU9250_FIFO_COUNT            0x72
#define MPU9250_FIFO_READ             0x74

// AK8963 registers
#define MPU9250_AK8963_I2C_ADDR       0x0C
#define MPU9250_AK8963_HXL            0x03
#define MPU9250_AK8963_CNTL1          0x0A
#define MPU9250_AK8963_PWR_DOWN       0x00
#define MPU9250_AK8963_CNT_MEAS1      0x12
#define MPU9250_AK8963_CNT_MEAS2      0x16
#define MPU9250_AK8963_FUSE_ROM       0x0F
#define MPU9250_AK8963_CNTL2          0x0B
#define MPU9250_AK8963_RESET          0x01
#define MPU9250_AK8963_ASA            0x10
#define MPU9250_AK8963_WHO_AM_I       0x00

/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */

/* Different options for basic MPU9250 setting registers */
typedef enum
{
	MPU9250_ADDRESS_0 = 0x68,
	MPU9250_ADDRESS_1 = 0x69
} MPU9250_address_t;

typedef enum
{
	MPU9250_ACCEL_RANGE_2G,
	MPU9250_ACCEL_RANGE_4G,
	MPU9250_ACCEL_RANGE_8G,
	MPU9250_ACCEL_RANGE_16G
} MPU9250_AccelRange_t;

typedef enum
{
	MPU9250_GYRO_RANGE_250DPS,
	MPU9250_GYRO_RANGE_500DPS,
	MPU9250_GYRO_RANGE_1000DPS,
	MPU9250_GYRO_RANGE_2000DPS
} MPU9250_GyroRange_t;

typedef enum
{
	MPU9250_DLPF_BANDWIDTH_184HZ,
	MPU9250_DLPF_BANDWIDTH_92HZ,
	MPU9250_DLPF_BANDWIDTH_41HZ,
	MPU9250_DLPF_BANDWIDTH_20HZ,
	MPU9250_DLPF_BANDWIDTH_10HZ,
	MPU9250_DLPF_BANDWIDTH_5HZ
} MPU9250_DlpfBandwidth_t;

typedef enum
{
	MPU9250_LP_ACCEL_ODR_0_24HZ  = 0,
	MPU9250_LP_ACCEL_ODR_0_49HZ  = 1,
	MPU9250_LP_ACCEL_ODR_0_98HZ  = 2,
	MPU9250_LP_ACCEL_ODR_1_95HZ  = 3,
	MPU9250_LP_ACCEL_ODR_3_91HZ  = 4,
	MPU9250_LP_ACCEL_ODR_7_81HZ  = 5,
	MPU9250_LP_ACCEL_ODR_15_63HZ = 6,
	MPU9250_LP_ACCEL_ODR_31_25HZ = 7,
	MPU9250_LP_ACCEL_ODR_62_50HZ = 8,
	MPU9250_LP_ACCEL_ODR_125HZ   = 9,
	MPU9250_LP_ACCEL_ODR_250HZ   = 10,
	MPU9250_LP_ACCEL_ODR_500HZ   = 11
} MPU9250_LpAccelOdr_t;

/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
#ifdef __cplusplus
}
#endif

/* ------------------------------ Module end ------------------------------- */
#endif

/* ------------------------------ File footer ------------------------------ */
