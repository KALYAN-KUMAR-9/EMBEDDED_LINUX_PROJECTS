#include "mpu6050_driver.h"

mpu6050_status	mpu6050_status_config = 
{
	.sample_div = 0,
	.dlpfvalue = 0
};


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
	msg[1].flags= I2C_M_RD;
	msg[1].len = len;
	msg[1].buf = pbuf;
	
	packets.msgs = &msg[0];
	packets.nmsgs = 2;

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
	mpu6050_status_config.sample_div = samplediv;
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
	mpu6050_status_config.dlpfvalue = dlpfval;
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
	int i;
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
		return ret_st;
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
		return ret_st;
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

int check_mpu6050_device(int fd)
{
	uint8_t regval;
	int ret_st = MPU6050_ioctl_read(fd, MPU6050_REG_WHO_AM_I,&regval, 1);

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

int set_mpu6050_reset_config(int fd)
{
	int ret_st = MPU6050_ioctl_write_mem(fd, MPU6050_REG_PWR_MGMT_1, MPU6050_DEVICE_RESET_BIT, 1);

        if(ret_st < 0)
        {
                perror(" IOCTL Byte Write Failed for power mgmt 1 config");
                return ret_st;
	}
	usleep(MPU6050_POWER_UP_TIME*1000);

	uint8_t data = (MPU6050_TEMP_RESET_BIT | MPU6050_ACCEL_RESET_BIT | MPU6050_GYRO_RESET_BIT);
	ret_st = MPU6050_ioctl_write_mem(fd, MPU6050_REG_SIGNAL_PATH_RESET, data, 1);

        if(ret_st < 0)
        {
                perror(" IOCTL Byte Write Failed for signal path reset config");
                return ret_st;
        }
        usleep(MPU6050_POWER_UP_TIME*1000);

	return 0;
}
static int is_mpu6050_sleep(int fd)
{
	uint8_t regval;
      	
	int ret_st = MPU6050_ioctl_read(fd, MPU6050_REG_PWR_MGMT_1,&regval, 1);
 
    	if(ret_st < 0)
        {
                 perror(" IOCTL Byte read Failed for pwr mgmt 1 ");
                 return ret_st;
        }
 
        if(regval &(1<<6))
        {
		printf("MPU6050 is in sleep\r\n");
        }
	else
	{
		printf("MPU6050 is not in sleep\r\n");
	}

	return 0;
}

int mpu6050_pwr_mgmt_1_write(int fd, mpu6050_pwr_mgmt_1_state pwr_mgmt_1_st, bool sleep)
{
	uint8_t data = 0;
	
	data = (uint8_t)pwr_mgmt_1_st.pwr_mgmt1;

	int ret_st = MPU6050_ioctl_write_mem(fd, MPU6050_REG_PWR_MGMT_1, data, 1);

        if(ret_st < 0)
        {
                perror(" IOCTL Byte Write Failed for MPU6050_REG_PWR_MGMT_1 config");
                return ret_st;
        }

	if((!pwr_mgmt_1_st.bits.Sleep) && (!sleep))
	{
		usleep(MPU6050_REG_UP_TIME_MIN);
	}
	return 0;
}

int mpu6050_pwr_mgmt_2_write(int fd, uint8_t data, mpu6050_gyroacl_stby_st gyroacl_st)
{
	uint8_t pwr_mgmt2 = 0;

	mpu6050_pwr_mgmt_1_state pwr_mgmt1_st = {
		.bits.Clock_Sel 	= 0,
		.bits.Temp_Dis 		= 0,
		.bits.Cycle		= 0,
		.bits.Sleep		= 0,
		.bits.Device_Reset	= 0
	};

	if(data & MPU6050_PWR_MGMT_2_GYRO_STBY)
	{
		if(gyroacl_st.gyro_pwr_mgmt_2_stby_en)
		{
			pwr_mgmt2 |= MPU6050_PWR_MGMT_2_GYRO_STBY;
		}
		else
		{
			pwr_mgmt2 &= ~MPU6050_PWR_MGMT_2_GYRO_STBY;
		}
	}

	if(data & MPU6050_PWR_MGMT_2_ACCL_STBY)
	{
		if(gyroacl_st.accl_pwr_mgmt_2_stby_en)
		{
			pwr_mgmt2 |= MPU6050_PWR_MGMT_2_ACCL_STBY;
		}
		else
		{
			pwr_mgmt2 &= ~MPU6050_PWR_MGMT_2_ACCL_STBY;
		}
	}

	if((data & MPU6050_PWR_MGMT_2_GYRO_STBY) && gyroacl_st.gyro_pwr_mgmt_2_stby_en)
	{
		pwr_mgmt1_st.bits.Clock_Sel 	= MPU6050_PWR_MGMT_1_CLOCK_PLL;		
	}
	else
	{
		pwr_mgmt1_st.bits.Clock_Sel = MPU6050_PWR_MGMT_1_CLOCK_INTERNAL;
	}
	
	if(pwr_mgmt1_st.bits.Clock_Sel == MPU6050_PWR_MGMT_1_CLOCK_PLL)
	{
		mpu6050_pwr_mgmt_1_write(fd, pwr_mgmt1_st, true);
		usleep(MPU6050_PLL_SETTING_TIME_MAX);
	}
	
	if(pwr_mgmt1_st.bits.Clock_Sel == MPU6050_PWR_MGMT_1_CLOCK_INTERNAL)
	{
		mpu6050_pwr_mgmt_1_write(fd, pwr_mgmt1_st, true);
	}
	int ret_st = MPU6050_ioctl_write_mem(fd, MPU6050_REG_PWR_MGMT_2, pwr_mgmt2, 1);
	
        if(ret_st < 0)
        {
                perror(" IOCTL Byte Write Failed for MPU6050_REG_PWR_MGMT_2 config");
                return ret_st;
        }
        return 0;
}

int mpu6050_i2c_init(int fd)
{
	int ret;

	ret = check_mpu6050_device(fd);
	if(ret)
	{
		return ret;
	}

	ret = set_mpu6050_reset_config(fd);
	if(ret)
	{
		return ret;
	}

	ret = is_mpu6050_sleep(fd);
	if(ret)
	{
		return ret;
	}

	mpu6050_pwr_mgmt_1_state pwr_mgmt1_st = {
		.bits.Clock_Sel 	= 0,
		.bits.Temp_Dis 		= true,
		.bits.Cycle		= 0,
		.bits.Sleep		= 0,
		.bits.Device_Reset	= 0
	};

	ret = mpu6050_pwr_mgmt_1_write(fd, pwr_mgmt1_st, false);
	if(ret)
	{
		return ret;
	}

	ret = set_mpu6050_gyro_config(fd, MPU6050_GYRO_FS_SEL0);
	if(ret)
	{
		return ret;
	}

	ret = set_mpu6050_lpf(fd, 200);
	if(ret)
	{
		return ret;
	}

	ret = set_mpu6050_samplediv(fd, MPU6050_FIFO_RATE_TO_DIVIDER(200));
	if(ret)
	{
		return ret;
	}

	ret = set_mpu6050_accel_config(fd, MPU6050_AFS_SEL0);
	if(ret)
	{
		return ret;
	}
	
	mpu6050_gyroacl_stby_st gyroaccl_st = 
	{
		.gyro_pwr_mgmt_2_stby_en = true,
		.accl_pwr_mgmt_2_stby_en = true
	};

	ret = mpu6050_pwr_mgmt_2_write(fd, (MPU6050_PWR_MGMT_2_GYRO_STBY| MPU6050_PWR_MGMT_2_ACCL_STBY), gyroaccl_st);
	if(ret)
	{
		return ret;
	}

	ret = mpu6050_pwr_mgmt_1_write(fd, pwr_mgmt1_st, false);
	if(ret)
	{
		return ret;
	}
	return 0;
}

int mpu6050_i2c_read_accelorometer(int fd, uint8_t *pbuf, uint32_t len)
{
	int ret = MPU6050_ioctl_read(fd, MPU6050_REG_ACCEL_XOUT_H, pbuf, len);

	if(ret < 0)
	{
		perror(" IOCTL Byte Write Failed for MPU6050_REG_ACCEL_XOUT_H read");
		return -1;
	}
	return 0;
}

int mpu6050_i2c_read_gyroscope(int fd, uint8_t *pbuf, uint32_t  len)
{
	int ret = MPU6050_ioctl_read(fd, MPU6050_REG_GYRO_XOUT_H, pbuf, len);

	if(ret < 0)
	{
		perror(" IOCTL Byte Write Failed for MPU6050_REG_GYRO_XOUT_H  read");
		return -1;
	}
	return 0;
}

int mpu6050_i2c_read_Temp(int fd, uint8_t *pbuf, uint32_t len)
{

	int ret = MPU6050_ioctl_read(fd, MPU6050_REG_TEMP_OUT_H, pbuf, len);

	if(ret < 0)
	{
		perror(" IOCTL Byte Write Failed for MPU6050_REG_TEMP_OUT_H read");
		return -1;
	}
	return 0;
}

int mpu6050_read(int fd, mpu6050_read_st gyroacl_read_st, uint8_t *pbuf)
{
	mpu6050_gyroacl_stby_st gyroaccl_st;
	unsigned int freq_hz = 0, period_us = 0;
	
	mpu6050_pwr_mgmt_1_state pwr_mgmt1_st;

	freq_hz = MPU6050_DIVIDER_TO_FIFO_RATE(mpu6050_status_config.sample_div);
	period_us = 1000000/freq_hz;
	printf("Calculated Sleep Budget  : %d us\n", period_us);

	switch(gyroacl_read_st)
	{
		case MPU6060_GYRO_RAW_READ:
			gyroaccl_st.gyro_pwr_mgmt_2_stby_en = false;

			 mpu6050_pwr_mgmt_2_write(fd,MPU6050_PWR_MGMT_2_GYRO_STBY, gyroaccl_st);
			usleep(period_us);
			
			mpu6050_i2c_read_gyroscope(fd, pbuf, 6);
			printf("mpu6050 gyro raw read completed\r\n");
			break;

		case MPU6050_ACCL_RAW_READ:
			gyroaccl_st.accl_pwr_mgmt_2_stby_en = false;

			mpu6050_pwr_mgmt_2_write(fd, MPU6050_PWR_MGMT_2_ACCL_STBY, gyroaccl_st);
			usleep(period_us);
			
			mpu6050_i2c_read_accelorometer(fd, pbuf, 6);
			printf("mpu6050 accl raw read completed\r\n");
			break;

		case MPU6050_GYRO_SCALE_READ:
			gyroaccl_st.gyro_pwr_mgmt_2_stby_en = false;

			 mpu6050_pwr_mgmt_2_write(fd, MPU6050_PWR_MGMT_2_GYRO_STBY, gyroaccl_st);
			usleep(period_us);

			mpu6050_i2c_read_gyroscope(fd, pbuf, 6);
			printf("mpu6050 gyro scale read completed\r\n");
			break;

		case MPU6050_ACCL_SCALE_READ:
			gyroaccl_st.accl_pwr_mgmt_2_stby_en = false;

			mpu6050_pwr_mgmt_2_write(fd, MPU6050_PWR_MGMT_2_ACCL_STBY, gyroaccl_st);
			usleep(period_us);
			
			mpu6050_i2c_read_accelorometer(fd, pbuf, 6);
			printf("mpu6050 accl scale read completed\r\n");
			break;

		case MPU6050_TEMP_RAW_READ:

			pwr_mgmt1_st.bits.Temp_Dis          = true;
			mpu6050_pwr_mgmt_1_write(fd, pwr_mgmt1_st, false);
			usleep(period_us);

			mpu6050_i2c_read_Temp(fd, pbuf, 2);
			printf("mpu6050 temp raw read completed\r\n");
			break;

		case MPU6050_TEMP_SCALE_READ:
				
			pwr_mgmt1_st.bits.Temp_Dis          = true;
			mpu6050_pwr_mgmt_1_write(fd, pwr_mgmt1_st, false);
			usleep(period_us);

			mpu6050_i2c_read_Temp(fd, pbuf, 2);
			printf("mpu6050 temp scale read completed\r\n");
		default:
			return -1;
	}
	return 0;
}
