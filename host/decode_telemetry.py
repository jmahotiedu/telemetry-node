#!/usr/bin/env python3
"""
Read telemetry frames from serial and print CSV (or write to file).
Usage: python decode_telemetry.py --port COM3 [--out data.csv] [--baud 115200]
"""
import argparse
import sys
from protocol import SYNC, FRAME_SIZE, decode_frame, find_sync

def main():
    ap = argparse.ArgumentParser(description="Decode telemetry stream from UART")
    ap.add_argument("--port", required=True, help="Serial port (e.g. COM3, /dev/ttyUSB0)")
    ap.add_argument("--baud", type=int, default=115200, help="Baud rate")
    ap.add_argument("--out", help="Optional: write CSV to file instead of stdout")
    args = ap.parse_args()

    try:
        import serial
    except ImportError:
        print("pip install pyserial", file=sys.stderr)
        sys.exit(1)

    out = open(args.out, "w") if args.out else sys.stdout
    out.write("timestamp_ms,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,temp_raw,vbat_raw\n")

    ser = serial.Serial(args.port, args.baud)
    buf = bytearray()
    errors = 0

    try:
        while True:
            buf += ser.read(ser.in_waiting or 1)
            start = find_sync(buf)
            if start == -1:
                if len(buf) > 2 * FRAME_SIZE:
                    buf = buf[-FRAME_SIZE:]
                continue
            if start > 0:
                buf = buf[start:]
            if len(buf) < FRAME_SIZE:
                continue
            frame_buf = bytes(buf[:FRAME_SIZE])
            buf = buf[FRAME_SIZE:]
            decoded = decode_frame(frame_buf)
            if decoded is None:
                errors += 1
                continue
            row = (
                f"{decoded['timestamp_ms']},{decoded['accel_x']},{decoded['accel_y']},{decoded['accel_z']},"
                f"{decoded['gyro_x']},{decoded['gyro_y']},{decoded['gyro_z']},"
                f"{decoded['temp_raw']},{decoded['vbat_raw']}\n"
            )
            out.write(row)
            out.flush()
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()
        if args.out:
            out.close()
        if errors:
            print(f"CRC/sync errors: {errors}", file=sys.stderr)


if __name__ == "__main__":
    main()
