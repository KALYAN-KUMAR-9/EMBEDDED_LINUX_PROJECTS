#ifndef MPU6050_DRIVER_H
#define MPU6050_DRIVER_H

/* * Datasheet refs
 * 1. MPU-6000 and MPU-6050 Product Specification Revision 3.4
 * 2. MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2
 */

/* Include header files*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define MPU6050_COMM_TYPE_I2C			0
#define MPU6050_COMM_TYPE_SPI			1
#define I2C_COMM				0
#define SPI_COMM				1
#define MPU6050_TYPE_I2C_SPI			I2C_COMM

#define ARRAY_SIZE(array)			(sizeof(array)/sizeof(array[0]))			

#define MPU6050_SLAVE_ADDR			0X68

#define MPU6050_REG_SELF_TEST_X			0X0D
#define MPU6050_REG_SELF_TEST_Y			0X0E
#define MPU6050_REG_SELF_TEST_Z			0X0F
#define MPU6050_REG_SELF_TEST_A			0X10

#define MPU6050_REG_SMPLRT_DIV			0X19
#define MPU6050_REG_CONFIG			0X1A
#define MPU6050_REG_GYRO_CONFIG			0X1B
#define MPU6050_REG_ACCEL_CONFIG		0X1C
#define MPU6050_REG_FIFO_EN			0X23

/* MPU 6050 I2C COMMUNICATION REGISTERS*/

#define MPU6050_REG_I2C_MST_CTRL		0X24

#define MPU6050_REG_I2C_SLV0_ADDR		0X25
#define MPU6050_REG_I2C_SLV0_REG		0X26
#define MPU6050_REG_I2C_SLV0_CTRL		0X27

#define MPU6050_REG_I2C_SLV1_ADDR		0X28
#define MPU6050_REG_I2C_SLV1_REG		0X29
#define MPU6050_REG_I2C_SLV1_CTRL		0X2A

#define MPU6050_REG_I2C_SLV2_ADDR		0X2B
#define MPU6050_REG_I2C_SLV2_REG		0X2C
#define MPU6050_REG_I2C_SLV2_CTRL		0X2D

#define MPU6050_REG_I2C_SLV3_ADDR		0X2E
#define MPU6050_REG_I2C_SLV3_REG		0X2F
#define MPU6050_REG_I2C_SLV3_CTRL		0X30

#define MPU6050_REG_I2C_SLV4_ADDR		0X31
#define MPU6050_REG_I2C_SLV4_REG		0X32
#define MPU6050_REG_I2C_SLV4_REG		0X33
#define MPU6050_REG_I2C_SLV4_CTRL		0X34
#define MPU6050_REG_I2C_SLV4_DI			0X35

#define MPU6050_REG_I2C_MST_STATUS		0X36
#define MPU6050_REG_INT_PIN_CFG			0X37
#define MPU6050_REG_INT_ENABLE			0X38
#define MPU6050_REG_INT_STATUS			0X3A

/* MPU 6050 ADDRESSES OF ACCELEROMETER X,Y,Z*/

#define MPU6050_REG_ACCEL_XOUT_H		0X3B
#define MPU6050_REG_ACCEL_XOUT_L		0X3C
#define MPU6050_REG_ACCEL_YOUT_H		0X3D
#define MPU6050_REG_ACCEL_YOUT_L		0X3E
#define MPU6050_REG_ACCEL_ZOUT_H		0X3F
#define MPU6050_REG_ACCEL_ZOUT_L		0X40

/* MPU 6050 ADDRESSES OF TEMPERATURE*/

#define MPU6050_REG_TEMP_OUT_H			0X41
#define MPU6050_REG_TEMP_OUT_L			0X42

/* MPU 6050 ADDRESSES OF GYROSCOPE X,Y,Z*/

#define MPU6050_REG_GYRO_XOUT_H			0X43
#define MPU6050_REG_GYRO_XOUT_L			0X44
#define MPU6050_REG_GYRO_YOUT_H			0X45
#define MPU6050_REG_GYRO_YOUT_L			0X46
#define MPU6050_REG_GYRO_ZOUT_H			0X47
#define MPU6050_REG_GYRO_ZOUT_L			0X48

#define MPU6050_REG_EXT_SENS_DATA_00		0X49
#define MPU6050_REG_EXT_SENS_DATA_01		0X4A
#define MPU6050_REG_EXT_SENS_DATA_02		0X4B
#define MPU6050_REG_EXT_SENS_DATA_03		0X4C
#define MPU6050_REG_EXT_SENS_DATA_04		0X4D
#define MPU6050_REG_EXT_SENS_DATA_05		0X4E
#define MPU6050_REG_EXT_SENS_DATA_06		0X4F
#define MPU6050_REG_EXT_SENS_DATA_07		0X50
#define MPU6050_REG_EXT_SENS_DATA_08		0X51
#define MPU6050_REG_EXT_SENS_DATA_09		0X52
#define MPU6050_REG_EXT_SENS_DATA_10		0X53
#define MPU6050_REG_EXT_SENS_DATA_11		0X54
#define MPU6050_REG_EXT_SENS_DATA_12		0X55
#define MPU6050_REG_EXT_SENS_DATA_13		0X56
#define MPU6050_REG_EXT_SENS_DATA_14		0X57
#define MPU6050_REG_EXT_SENS_DATA_15		0X58
#define MPU6050_REG_EXT_SENS_DATA_16		0X59
#define MPU6050_REG_EXT_SENS_DATA_17		0X5A
#define MPU6050_REG_EXT_SENS_DATA_18		0X5B
#define MPU6050_REG_EXT_SENS_DATA_19		0X5C
#define MPU6050_REG_EXT_SENS_DATA_20		0X5D
#define MPU6050_REG_EXT_SENS_DATA_21		0X5E
#define MPU6050_REG_EXT_SENS_DATA_22		0X5F
#define MPU6050_REG_EXT_SENS_DATA_23		0X60

/* MPU 6050 I2C SLAVE DATE OUT REGISTER ADDRESSES*/

#define MPU6050_REG_I2C_SLV0_DO			0X63
#define MPU6050_REG_I2C_SLV1_DO			0X64
#define MPU6050_REG_I2C_SLV2_DO			0X65
#define MPU6050_REG_I2C_SLV3_DO			0X66

#define MPU6050_REG_I2C_MST_DELAY_CTRL		0X67
#define MPU6050_REG_SIGNAL_PATH_RESET		0X68

#define MPU6050_REG_USER_CTRL			0X6A
#define MPU6050_REG_PWR_MGMT_1			0X6B
#define MPU6050_REG_PWR_MGMT_2			0X6C
#define MPU6050_REG_FIFO_COUNTH			0X72
#define MPU6050_REG_FIFO_COUNTL			0X73
#define MPU6050_REG_FIFO_R_W			0X74
#define MPU6050_REG_WHO_AM_I			0X75

#define MPU6050_GYRO_FS_SEL_0			131
#define MPU6050_GYRO_FS_SEL_1          		65.5
#define MPU6050_GYRO_FS_SEL_2         		32.8
#define MPU6050_GYRO_FS_SEL_3          		16.4

#define MPU6050_ACCEL_AFS_SEL_0			16384
#define MPU6050_ACCEL_AFS_SEL_1        		8192
#define MPU6050_ACCEL_AFS_SEL_2        		4096
#define MPU6050_ACCEL_AFS_SEL_3        		2048

#define MPU6050_GYRO_CONFIG_FSR_SHIFT   	3
#define MPU6050_ACCL_CONFIG_FSR_SHIFT    	3

typedef enum
{
	MPU6050_AFS_SEL = 0,
	MPU6050_AFS_SEL = 1,
	MPU6050_AFS_SEL = 2,
	MPU6050_AFS_SEL = 3,

}mpu6050_accel_full_scale_e;

typedef enum
{
	MPU6050_GYRO_FS_SEL = 0,
	MPU6050_GYRO_FS_SEL = 1,
	MPU6050_GYRO_FS_SEL = 2,
	MPU6050_GYRO_FS_SEL = 3,

}mpu6050_gyro_full_scale_e;

typedef enum
{
	MPU6050_DLPF_NOLPF2 = 0,
	MPU6050_DLPF_200HZ,
	MPU6050_DLPF_100HZ,
	MPU6050_DLPF_45HZ,
	MPU6050_DLPF_20HZ,
	MPU6050_DLPF_10HZ,
	MPU6050_DLPF_5HZ

}mpu6050_dlpf_e

typedef struct
{
	double ACC_X;
	double ACC_Y;
	double ACC_Z;
}mpu6050_acc_read_t;

typedef struct
{
	double GYRO_X;
	double GYRO_Y;
	double GYRO_Z;
}mpu6050_gyro_read_t

#if(MPU6050_TYPE_I2C_SPI == I2C_COMM)
void MPU6050_i2c_initilization(void);
void MPU6050_i2c_read_accelorometer(signed short int *pbuf);
void MPU6050_i2c_read_gyroscope(signed short int *pbuf);
int MPU6050_ioctl_write_mem(int fd, uint8_t addr, uint8_t data, uint32_t len);
int MPU6050_ioctl_read(int fd, uint8_t addr, uint8_t *pbuf, uint32_t len);
void set_MPU6050_slave_Address(uint8_t slave_addr);
int set_mpu6050_lpf(int fd, uint8_t sampling_rate);
int set_mpu6050_samplediv(int fd, uint8_t samplediv);
int set_mpu6050_dlpf_reg(int fd, uint8_t dlpfval);
int set_mpu6050_gyro_config(int fd, uint8_t data);
int set_mpu6050_accel_config(int fd, uint8_t data);
int check_mpu6050_device(int fd, uint8_t data);
#endif

#endif /*MPU6050_DRIVER_H*/

