# imu-node-stm32

STM32F401RE firmware for the IMU node (MeRo1 Project).
Reads LSM6DS33 IMU via I²C and transmits data over UART.

## Hardware

| Component        | Details                        |
|------------------|--------------------------------|
| MCU              | STM32F401RE Nucleo             |
| IMU              | LSM6DS33 (I²C, addr 0x6A)     |
| I²C              | I2C1, PB8=SCL, PB9=SDA        |
| UART             | USART2, PA2=TX, 115200 Baud   |

## Wiring

| LSM6DS33 | STM32F401RE        |
|----------|--------------------|
| VDD      | 3.3V               |
| GND      | GND                |
| SDA      | PB9                |
| SCL      | PB8                |
| SDO      | GND (addr = 0x6A)  |
| CS       | 3.3V (I²C mode)    |

> Pull-up resistors 4.7 kΩ required on SDA and SCL.

## Configuration

| Parameter     | Value         |
|---------------|---------------|
| Accel range   | ±4 g          |
| Gyro range    | ±500 °/s      |
| ODR           | 104 Hz        |
| Output rate   | 50 Hz (UART)  |

## Output Format

AX:%.4f AY:%.4f AZ:%.4f GX:%.4f GY:%.4f GZ:%.4f
Units: Accel in m/s², Gyro in rad/s.

## Build

- STM32CubeIDE
- HAL + FreeRTOS (CMSIS_V1, native API)
- Linker flag required: `-u _printf_float`

## Status

- [x] LSM6DS33 I²C driver
- [x] UART output at 50 Hz
- [ ] External pull-up resistors (currently using internal ~40 kΩ)
- [ ] Gyro bias calibration
