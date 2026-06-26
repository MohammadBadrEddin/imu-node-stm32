#ifndef LSM6DS33_H
#define LSM6DS33_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

//I2C-Adress 7-bit, but HAL uses 8-bit left-shift

#define LSM6DS33_AD					(0x6A << 1)

//Register adresses
#define LSM6DS33_WHO_AM_I			0x0F
#define LSM6DS33_WHO_AM_I_VAL		0x69

#define LSM6DS33_CTRL1_XL			0x10 // accel control
#define LSM6DS33_CTRL2_G			0x11 // Gyro control
#define LSM6DS33_CTRL3_C			0x12

#define LSM6DS33_OUTX_L_G			0x22
#define LSM6DS33_OUTX_L_XL			0x28

/* Configuration:
Register table from datasheet
 */

#define LSM6DS33_CTRL1_XL_VAL   	0x48
#define LSM6DS33_CTRL2_G_VAL    	0x44
#define LSM6DS33_CTRL3_C_VAL    	0x44

//Sensitivities
#define LSM6DS33_ACCEL_SENS_4G  	0.000122f  /* g/LSB --> +/-4g */
#define LSM6DS33_GYRO_SENS_500  	0.01750f   /* d/LSB --> +/-500dps */

#define G_TO_MS2                	9.80665f // G
#define DEG_TO_RAD              	0.017453293f // PI/180


typedef enum {
    LSM6DS33_OK    = 0,
    LSM6DS33_ERROR = 1
} LSM6DS33_Status;

typedef struct {
    float ax, ay, az;   /* m/s^2 */
    float gx, gy, gz;   /* rad/s */
} LSM6DS33_Data;

/* Function decl. */
LSM6DS33_Status LSM6DS33_Init(I2C_HandleTypeDef *hi2c);
LSM6DS33_Status LSM6DS33_ReadData(I2C_HandleTypeDef *hi2c, LSM6DS33_Data *data);

#endif /* LSM6DS33_H */
