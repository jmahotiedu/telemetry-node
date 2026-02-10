#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define PAYLOAD_SIZE 20
#define FRAME_SIZE   (2 + 1 + PAYLOAD_SIZE + 2)

typedef struct {
    uint32_t timestamp_ms;
    int16_t  accel_x, accel_y, accel_z;
    int16_t  gyro_x, gyro_y, gyro_z;
    uint16_t temp_raw, vbat_raw;
} sample_t;

uint16_t protocol_crc16(const uint8_t *data, size_t len);
size_t   protocol_build_frame(const sample_t *s, uint8_t *out, size_t out_max);

#ifdef __cplusplus
}
#endif

#endif
