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

Deterministic fixture used in `host/tests/test_protocol.py`:

```
aa5514e8030000640038ff2c010a001400fbff00020004449d
```

Decoded fields for that frame:

- `timestamp_ms=1000`
- `accel_x=100`, `accel_y=-200`, `accel_z=300`
- `gyro_x=10`, `gyro_y=20`, `gyro_z=-5`
- `temp_raw=512`, `vbat_raw=1024`

## Host serial paths

Windows:

- Typical port: `COM3` (or any `COMx` assigned in Device Manager)
- Example: `python decode_telemetry.py --port COM3 --out data.csv`

Linux:

- Typical USB serial paths: `/dev/ttyUSB0` or `/dev/ttyACM0`
- Example: `python decode_telemetry.py --port /dev/ttyUSB0 --out data.csv`
