# Telemetry wire protocol

## Frame layout

| Field    | Size  | Description                    |
|----------|-------|--------------------------------|
| Sync     | 2 B   | `0xAA 0x55`                    |
| Length   | 1 B   | Payload length = 20           |
| Payload  | 20 B  | See below                      |
| CRC16    | 2 B   | Little-endian, over payload    |

All multi-byte values are **little-endian**.

## Payload (20 bytes)

| Offset | Size | Type    | Description        |
|--------|------|---------|--------------------|
| 0      | 4    | uint32  | timestamp_ms       |
| 4      | 2    | int16   | accel_x            |
| 6      | 2    | int16   | accel_y            |
| 8      | 2    | int16   | accel_z            |
| 10     | 2    | int16   | gyro_x             |
| 12     | 2    | int16   | gyro_y             |
| 14     | 2    | int16   | gyro_z             |
| 16     | 2    | uint16  | temp_raw (ADC)     |
| 18     | 2    | uint16  | vbat_raw (ADC)     |

## CRC-16

- Polynomial: CRC-16-IBM (0xA001), initial 0xFFFF; over **payload only** (20 bytes).
- Transmitted as 2 bytes, little-endian (LSB first).

## Example (hex)

Sync + len(20) + 20-byte payload + CRC16:

```
AA 55 14 [20 bytes payload] XX XX
```
