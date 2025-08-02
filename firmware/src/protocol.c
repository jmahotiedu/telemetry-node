#include "protocol.h"
#include <string.h>

#define SYNC_0 0xAA
#define SYNC_1 0x55

/* CRC-16-IBM (0xA001), initial 0xFFFF */
uint16_t protocol_crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    while (len--) {
        crc ^= (uint16_t)*data++;
        for (int i = 0; i < 8; i++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

size_t protocol_build_frame(const sample_t *s, uint8_t *out, size_t out_max) {
    if (out_max < FRAME_SIZE) return 0;

    out[0] = SYNC_0;
    out[1] = SYNC_1;
    out[2] = (uint8_t)PAYLOAD_SIZE;

    uint8_t *p = out + 3;
    *(uint32_t *)p = s->timestamp_ms; p += 4;
    *(int16_t *)p = s->accel_x; p += 2;
    *(int16_t *)p = s->accel_y; p += 2;
    *(int16_t *)p = s->accel_z; p += 2;
    *(int16_t *)p = s->gyro_x;  p += 2;
    *(int16_t *)p = s->gyro_y;  p += 2;
    *(int16_t *)p = s->gyro_z;  p += 2;
    *(uint16_t *)p = s->temp_raw; p += 2;
    *(uint16_t *)p = s->vbat_raw; p += 2;

    uint16_t crc = protocol_crc16(out + 3, PAYLOAD_SIZE);
    out[3 + PAYLOAD_SIZE]     = (uint8_t)(crc & 0xFF);
    out[3 + PAYLOAD_SIZE + 1] = (uint8_t)(crc >> 8);

    return FRAME_SIZE;
}
