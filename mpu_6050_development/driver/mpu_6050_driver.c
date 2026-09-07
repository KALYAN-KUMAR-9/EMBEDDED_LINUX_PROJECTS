#include "mpu_6050_driver.h"

int MPU6050_ioctl_write_mem(int fd, uint8_t addr, uint8_t data, uint32_t len)
{
	struct i2c_msg msg;
    	struct i2c_rdwr_ioctl_data packets;
	uint8_t tx_buf[2];

	tx_buf[0] = addr;
	tx_buf[1] = data;

	msg.addr = MPU6050_SLAVE_ADDR;
	msg.flags = 0;
	msg.len = len+1;
	msg.buf = tx_buf;

	packets.msgs  = &msg;
	packets.nmsgs = 1;

	int ret = ioctl(fd, I2C_RDWR, &packets);
	if (ret < 0) 
	{
        	perror(" IOCTL Byte Write Failed");
	        return -1;
    	}
    return 0;
}

int MPU6050_ioctl_read(int fd, uint8_t addr, uint8_t *pbuf, uint32_t len)
{
	struct i2c_msg msg[2];
	struct i2c_rdwr_ioctl_data packets;

	msg[0].addr = MPU6050_SLAVE_ADDR;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &addr;

	msg[1].addr = MPU6050_SLAVE_ADDR;
	msg[1].flag = I2C_M_RD;
	msg[1].len = len;
	msg[1].buf = pbuf;
	
	packets.msgs = &msg;
	packet.nmsgs = 2;

	int ret = ioctl(fd, I2C_RDWR, &packets);
	if(ret < 0)
	{
		perror("Pure IOCTL Multi-Byte Burst Read Failed");
        	return -1;
	}

	return 0;
}

int set_mpu6050_samplediv(int fd, uint8_t samplediv)
{
	int ret_st = MPU6050_ioctl_write_mem(fd, MPU6050_REG_SMPLRT_DIV, samplediv, 1);

	if(ret_st < 0)
	{
		perror(" IOCTL Byte Write Failed for samplediv");
		return ret_st;
	}
	return 0;
}

int set_mpu6050_dlpf_reg(int fd, uint8_t dlpfval)
{
	int ret_st = MPU6050_ioctl_write_mem(fd, MPU6050_REG_CONFIG, dlpfval, 1);
	if(ret_st < 0)
	{
		perror(" IOCTL Byte Write Failed for dlpfval");
                return ret_st;
 	}      
	return 0;
}

int set_mpu6050_lpf(int fd, uint8_t sampling_rate)
{
	static const int niq_hz[] = {400, 200, 90, 40, 20, 10};
	static const int dlpf[] = {
		MPU6050_DLPF_200HZ, MPU6050_DLPF_100HZ,
		MPU6050_DLPF_45HZ, MPU6050_DLPF_20HZ,
		MPU6050_DLPF_10HZ, MPU6050_DLPF_5HZ
	};
	int i, result;
	uint8_t data;

	data = MPU6050_DLPF_5HZ;
	for (i = 0; i < ARRAY_SIZE(niq_hz); ++i) 
	{
		if (sampling_rate >= niq_hz[i]) 
		{
			data = dlpf[i];
			break;
		}
	}

	int ret_st = set_mpu6050_dlpf_reg(fd, data);

	if(ret_st < 0)
	{
		return ret_St;
	}
	return 0;
}

int set_mpu6050_gyro_config(int fd, uint8_t data)
{
	data = (data << MPU6050_GYRO_CONFIG_FSR_SHIFT);
	data = (data & (0X18));

	int ret_st = MPU6050_ioctl_write_mem(fd, MPU6050_REG_GYRO_CONFIG, data, 1);

	if(ret_st < 0)
	{
		perror(" IOCTL Byte Write Failed for gyro config");
		return ret_St;
	}
	return 0;
}

int set_mpu6050_accel_config(int fd, uint8_t data)
{
	data = (data << MPU6050_ACCL_CONFIG_FSR_SHIFT);
	data = (data & (0X18));

	int ret_st = MPU6050_ioctl_write_mem(fd, MPU6050_REG_ACCEL_CONFIG, data, 1);

	if(ret_st < 0)
	{
		perror(" IOCTL Byte Write Failed for accel config");
		return ret_st;
	}
	return 0;
}

int check_mpu6050_device(int fd, uint8_t data)
{
	uint8_t regval;
	int ret_St = MPU6050_ioctl_read(fd, MPU6050_REG_WHO_AM_I,&regval, 1);

	if(ret_st < 0)
	{
		perror(" IOCTL Byte read Failed for who am i reg read");
		return ret_st;
	}

	if((regval&(0X7E)) == MPU6050_SLAVE_ADDR)
	{
		printf("MPU6050 slave address matched");
	}

	return 0;
}
