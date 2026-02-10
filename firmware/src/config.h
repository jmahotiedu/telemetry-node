#ifndef CONFIG_H
#define CONFIG_H

#define UART_BAUD           115200
#define UART_TX_PIN         17
#define UART_RX_PIN         16

#define SAMPLE_QUEUE_LEN    8
#define SAMPLE_RATE_MS      10

#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22
#define MPU6050_I2C_ADDR    0x68

#define VBAT_ADC_PIN        34
#if defined(ARDUINO_ARCH_ESP32)
#define VBAT_ADC_ATTEN      ADC_11db
#endif
#define VBAT_DIVIDER_RATIO  2.0f

#endif
