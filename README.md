# Telemetry Node

Real-time embedded telemetry logger: ESP32 + FreeRTOS samples IMU, temperature, and battery voltage at a fixed rate and streams binary frames over UART. A Python host tool decodes the stream and writes CSV.

Resume bullet: Designed and implemented a real-time telemetry logger (ESP32, FreeRTOS) with fixed-rate multi-sensor sampling, CRC-protected binary protocol, and Python decode/export tooling.

## Hardware

- ESP32 dev board (for example ESP32-WROOM-32)
- MPU6050 IMU (I2C, 3.3 V)
- Optional battery divider to ADC pin
- ESP32 internal temperature sensor

## Wiring

| ESP32 | MPU6050 |
|---|---|
| GPIO 21 (SDA) | SDA |
| GPIO 22 (SCL) | SCL |
| 3.3 V | VCC |
| GND | GND |

Battery (optional): divider to GPIO 34 (ADC1_6). Update pin config in `firmware/src/config.h` if needed.

## Firmware

```bash
cd firmware
pio run
pio run -t upload
```

UART defaults to `115200 8N1`.

## Host Tool

```powershell
cd host
.\setup_venv.ps1
.\run_test.ps1
.\.venv\Scripts\python.exe decode_telemetry.py --port COM3 --out data.csv
```

Linux/macOS serial examples:

```bash
python decode_telemetry.py --port /dev/ttyUSB0 --out data.csv
python decode_telemetry.py --port /dev/ttyACM0 --out data.csv
```

## Protocol

Frame format:

- Sync: `0xAA 0x55`
- Length: `1 byte` (payload length = 20)
- Payload: timestamp + accel/gyro/temp/vbat fields
- CRC16: `2 bytes` little-endian over payload bytes

See `docs/protocol.md` for full layout and deterministic fixture bytes used in host tests.

## Verification

1. Host protocol checks (no hardware):
   - `python -m unittest discover -s host/tests -p "test_*.py"`
   - `python -m ruff check host`
2. Firmware + host stream:
   - Flash firmware, then run decoder with your serial port.
   - Confirm CSV rows arrive around the configured sample interval.

## CI

GitHub Actions runs:

- ESP32 firmware build via PlatformIO
- Host dependency install
- Host static check (`ruff`)
- Host protocol tests (`unittest`)

## License

MIT
