# Telemetry Node

Real-time embedded telemetry logger: ESP32 + FreeRTOS samples IMU, temperature, and battery voltage at a fixed rate and streams binary frames over UART. A Python host tool decodes the stream and outputs CSV.

**Resume bullet:** Designed and implemented real-time telemetry logger (ESP32, FreeRTOS): fixed-rate multi-sensor sampling, binary wire protocol with CRC, Python host tool for decode and CSV export.

## Hardware

- **ESP32** dev board (e.g. ESP32-WROOM-32 with USB-serial)
- **MPU6050** IMU (I2C, 3.3 V)
- **Battery** voltage: divider to ADC pin (optional; for bench leave unconnected or tie to 3.3 V)
- **Temp:** ESP32 internal temperature sensor (no extra hardware)

## Wiring

| ESP32   | MPU6050   |
|---------|-----------|
| GPIO 21 (SDA) | SDA      |
| GPIO 22 (SCL) | SCL      |
| 3.3 V   | VCC       |
| GND     | GND       |

Battery (optional): voltage divider (e.g. 2:1) to **GPIO 34** (ADC1_6, 3.3 V max input). Edit `firmware/src/config.h` to change pins.

## Firmware (ESP32)

```bash
cd firmware
pio run
pio run -t upload
```

UART: 115200 8N1, TX/RX to USB (default pins on most dev boards).

## Host tool (Python)

**Using a venv (recommended):**

```powershell
cd host
.\setup_venv.ps1                    # creates .venv and installs pyserial
.\.venv\Scripts\Activate.ps1        # activate (optional; run_test uses venv automatically)
.\run_test.ps1                      # run protocol unit test
.\.venv\Scripts\python.exe decode_telemetry.py --port COM3
```

**Without venv:** `pip install -r requirements.txt` then `python decode_telemetry.py --port COM3` (or `/dev/ttyUSB0` on Linux). Optional: `--out data.csv`.

## Protocol

Binary frames: sync `0xAA 0x55` | length (1 B) | payload (20 B) | CRC16 (2 B). See [docs/protocol.md](docs/protocol.md).

## Verification

1. **Host only:** `cd host && python protocol.py` — encode/decode unit test (no hardware).
2. **Firmware + host:** Flash firmware, connect USB, run `python decode_telemetry.py --port COMx`. You should see CSV rows at ~100 Hz. Without MPU6050 wired, accel/gyro are zero; temp and vbat are from ESP32 internal sensor and ADC.

## Design

Fixed-rate multi-sensor sampling (FreeRTOS timer), binary wire protocol with sync and CRC16. See [docs/protocol.md](docs/protocol.md) for frame format.

## Pushing to GitHub

Create a new repository on GitHub (e.g. `jmahotiedu/telemetry-node`), then:

```bash
git remote add origin https://github.com/jmahotiedu/telemetry-node.git
git push -u origin master
```

CI runs on push: firmware build (PlatformIO) and host protocol unit test (Python).

## License

MIT
