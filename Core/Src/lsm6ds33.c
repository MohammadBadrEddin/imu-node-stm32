#include "lsm6ds33.h"
#include "FreeRTOS.h"
#include "task.h"


static LSM6DS33_Status WriteReg(I2C_HandleTypeDef *hi2c,
                                 uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    if (HAL_I2C_Master_Transmit(hi2c, LSM6DS33_AD,
                                 buf, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        return LSM6DS33_ERROR;
    }
    return LSM6DS33_OK;
}

/* (auto-increment via IF_INC) */
static LSM6DS33_Status ReadRegs(I2C_HandleTypeDef *hi2c,
                                  uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (HAL_I2C_Master_Transmit(hi2c, LSM6DS33_AD,
                                 &reg, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        return LSM6DS33_ERROR;
    }
    if (HAL_I2C_Master_Receive(hi2c, LSM6DS33_AD,
                                buf, len, HAL_MAX_DELAY) != HAL_OK)
    {
        return LSM6DS33_ERROR;
    }
    return LSM6DS33_OK;
}

/* ------------------------------------------------------------------ */

LSM6DS33_Status LSM6DS33_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t who_am_i = 0;

    if (ReadRegs(hi2c, LSM6DS33_WHO_AM_I, &who_am_i, 1) != LSM6DS33_OK)
        return LSM6DS33_ERROR;

    if (who_am_i != LSM6DS33_WHO_AM_I_VAL)
        return LSM6DS33_ERROR;

    /* 2. Software-Reset */
    /* CTRL3_C bit0 = SW_RESET — Sensor needs ~1ms to reset*/
    if (WriteReg(hi2c, LSM6DS33_CTRL3_C, 0x01) != LSM6DS33_OK)
        return LSM6DS33_ERROR;
    vTaskDelay(pdMS_TO_TICKS(5));  /* before Init wait*/

    /* 3. CTRL3_C: BDU=1, IF_INC=1 */
    if (WriteReg(hi2c, LSM6DS33_CTRL3_C, LSM6DS33_CTRL3_C_VAL) != LSM6DS33_OK)
        return LSM6DS33_ERROR;

    /* 4. Gyro config (CTRL2_G) */
    if (WriteReg(hi2c, LSM6DS33_CTRL2_G, LSM6DS33_CTRL2_G_VAL) != LSM6DS33_OK)
        return LSM6DS33_ERROR;

    /* 5. Accel config(CTRL1_XL) */
    if (WriteReg(hi2c, LSM6DS33_CTRL1_XL, LSM6DS33_CTRL1_XL_VAL) != LSM6DS33_OK)
        return LSM6DS33_ERROR;

    /* 6. Sensor needs ~10ms after ODR activation*/
    vTaskDelay(pdMS_TO_TICKS(10));

    return LSM6DS33_OK;
}

/* ------------------------------------------------------------------ */

LSM6DS33_Status LSM6DS33_ReadData(I2C_HandleTypeDef *hi2c, LSM6DS33_Data *data)
{
    uint8_t raw[12];  /* 6 Bytes Gyro + 6 Bytes Accel */
    int16_t raw_g[3], raw_a[3];

    /*
     * Gyro-Register: OUTX_L_G (0x22) to OUTZ_H_G (0x27) → 6 Bytes
     * IF_INC set (CTRL3_C)
     */
    if (ReadRegs(hi2c, LSM6DS33_OUTX_L_G, &raw[0], 6) != LSM6DS33_OK)
        return LSM6DS33_ERROR;

    /*
     * Accel-Register: OUTX_L_XL (0x28) to OUTZ_H_XL (0x2D) → 6 Bytes
     */
    if (ReadRegs(hi2c, LSM6DS33_OUTX_L_XL, &raw[6], 6) != LSM6DS33_OK)
        return LSM6DS33_ERROR;

    /* Bytes to 16-Bit signed (Little-Endian: Low first!) */
    raw_g[0] = (int16_t)((raw[1]  << 8) | raw[0]);   /* GX */
    raw_g[1] = (int16_t)((raw[3]  << 8) | raw[2]);   /* GY */
    raw_g[2] = (int16_t)((raw[5]  << 8) | raw[4]);   /* GZ */

    raw_a[0] = (int16_t)((raw[7]  << 8) | raw[6]);   /* AX */
    raw_a[1] = (int16_t)((raw[9]  << 8) | raw[8]);   /* AY */
    raw_a[2] = (int16_t)((raw[11] << 8) | raw[10]);  /* AZ */

    /* recalc: raw → physical units*/
    /* Accel: raw * sens[g/LSB] * G_TO_MS2 → m/s² */
    data->ax = (float)raw_a[0] * LSM6DS33_ACCEL_SENS_4G * G_TO_MS2;
    data->ay = (float)raw_a[1] * LSM6DS33_ACCEL_SENS_4G * G_TO_MS2;
    data->az = (float)raw_a[2] * LSM6DS33_ACCEL_SENS_4G * G_TO_MS2;

    /* Gyro: raw * sens[°/LSB] * DEG_TO_RAD → rad/s */
    data->gx = (float)raw_g[0] * LSM6DS33_GYRO_SENS_500 * DEG_TO_RAD;
    data->gy = (float)raw_g[1] * LSM6DS33_GYRO_SENS_500 * DEG_TO_RAD;
    data->gz = (float)raw_g[2] * LSM6DS33_GYRO_SENS_500 * DEG_TO_RAD;

    return LSM6DS33_OK;
}
