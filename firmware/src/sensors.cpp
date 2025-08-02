/**
 * Sensor reads: MPU6050 (I2C), ESP32 internal temp, battery ADC.
 */
#include "sensors.h"
#include "config.h"
#include <Wire.h>
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <esp32-hal-adc.h>
extern float temperatureRead(void);
#endif

#define MPU6050_REG_PWR_MGMT_1  0x6B
#define MPU6050_REG_ACCEL_OUT  0x3B
#define MPU6050_READ_LEN       14

static uint8_t mpu_ok = 0;

static int16_t read16_be(uint8_t *p) {
    return (int16_t)((p[0] << 8) | p[1]);
}

void sensors_init(void) {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    Wire.beginTransmission(MPU6050_I2C_ADDR);
    Wire.write(MPU6050_REG_PWR_MGMT_1);
    Wire.write(0);
    if (Wire.endTransmission(true) == 0)
        mpu_ok = 1;

#if defined(ARDUINO_ARCH_ESP32)
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    adcAttachPin(VBAT_ADC_PIN);
#endif
}

void sensors_read(sample_t *s) {
    if (!s) return;

    if (mpu_ok) {
        Wire.beginTransmission(MPU6050_I2C_ADDR);
        Wire.write(MPU6050_REG_ACCEL_OUT);
        if (Wire.endTransmission(false) == 0 && Wire.requestFrom((uint8_t)MPU6050_I2C_ADDR, (uint8_t)MPU6050_READ_LEN) >= MPU6050_READ_LEN) {
            uint8_t buf[MPU6050_READ_LEN];
            for (int i = 0; i < MPU6050_READ_LEN; i++)
                buf[i] = Wire.read();
            s->accel_x = read16_be(buf + 0);
            s->accel_y = read16_be(buf + 2);
            s->accel_z = read16_be(buf + 4);
            s->gyro_x  = read16_be(buf + 8);
            s->gyro_y  = read16_be(buf + 10);
            s->gyro_z  = read16_be(buf + 12);
        } else {
            s->accel_x = s->accel_y = s->accel_z = 0;
            s->gyro_x = s->gyro_y = s->gyro_z = 0;
        }
    } else {
        s->accel_x = s->accel_y = s->accel_z = 0;
        s->gyro_x = s->gyro_y = s->gyro_z = 0;
    }

#if defined(ARDUINO_ARCH_ESP32)
    float t = temperatureRead();
    s->temp_raw = (uint16_t)(t * 100.0f);
#else
    s->temp_raw = 0;
#endif

#if defined(VBAT_ADC_PIN)
    s->vbat_raw = (uint16_t)analogRead(VBAT_ADC_PIN);
#else
    s->vbat_raw = 0;
#endif
}
