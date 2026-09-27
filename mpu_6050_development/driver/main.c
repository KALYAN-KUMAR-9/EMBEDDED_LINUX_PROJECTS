#include "mpu6050_driver.h"

#define I2C_DEVICE_FILE   "/dev/i2c-2"

int main(void)
{
	int fd;

	uint8_t accl_buf[6] = {0,};
       	uint8_t gyro_buf[6] = {0,};
	short int accl_16bit_val[3];
	short int gyro_16bit_val[3];

	/*first lets open the I2C device file */
    if ((fd = open(I2C_DEVICE_FILE,O_RDWR)) < 0) {
        perror("Failed to open I2C device file.\n");
        return -1;
    }

    /*set the I2C slave address using ioctl I2C_SLAVE command */
    if (ioctl(fd,I2C_SLAVE,MPU6050_SLAVE_ADDR) < 0) {
            perror("Failed to set I2C slave address.\n");
            close(fd);
            return -1;
    }

    mpu6050_i2c_init(fd);

    while(1)
    {
	mpu6050_read(fd, MPU6060_GYRO_RAW_READ, gyro_buf);
	mpu6050_read(fd, MPU6050_ACCL_RAW_READ, accl_buf);
	
	accl_16bit_val[0] = ((accl_buf[0]<<8) | (accl_buf[1]));
	accl_16bit_val[1] = ((accl_buf[2]<<8) | (accl_buf[3]));
	accl_16bit_val[2] = ((accl_buf[4]<<8) | (accl_buf[5]));

	gyro_16bit_val[0] = ((gyro_buf[0]<<8) | (gyro_buf[1]));
	gyro_16bit_val[1] = ((gyro_buf[2]<<8) | (gyro_buf[3]));
	gyro_16bit_val[2] = ((gyro_buf[4]<<8) | (gyro_buf[5]));

	printf("Acc(raw)=> X:%d\t, Y:%d\t, Z:%d\t, gyro(raw)=> X:%d\t, Y:%d\t,  Z:%d\r\n", accl_16bit_val[0], accl_16bit_val[1], accl_16bit_val[2], gyro_16bit_val[0], gyro_16bit_val[1], gyro_16bit_val[2]);

	usleep(100*1000);
    }
	return 0;
}
