"""
Binary telemetry protocol: encode/decode frames and CRC16.
Frame: SYNC (2) | len (1) | payload (20) | crc16 (2). Little-endian.
"""
import struct

SYNC = bytes([0xAA, 0x55])
PAYLOAD_SIZE = 20
FRAME_SIZE = 2 + 1 + PAYLOAD_SIZE + 2  # 25 bytes

# CRC-16-IBM (0xA001), initial 0xFFFF
def crc16(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc


def encode_frame(
    timestamp_ms: int,
    accel_x: int, accel_y: int, accel_z: int,
    gyro_x: int, gyro_y: int, gyro_z: int,
    temp_raw: int, vbat_raw: int,
) -> bytes:
    payload = struct.pack(
        "<IhhhhhhHH",
        timestamp_ms & 0xFFFFFFFF,
        accel_x, accel_y, accel_z,
        gyro_x, gyro_y, gyro_z,
        temp_raw & 0xFFFF, vbat_raw & 0xFFFF,
    )
    assert len(payload) == PAYLOAD_SIZE
    crc = crc16(payload)
    return SYNC + bytes([PAYLOAD_SIZE]) + payload + struct.pack("<H", crc)


def decode_payload(data: bytes) -> dict:
    if len(data) != PAYLOAD_SIZE:
        raise ValueError(f"payload length {len(data)} != {PAYLOAD_SIZE}")
    (timestamp_ms, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temp_raw, vbat_raw) = struct.unpack(
        "<IhhhhhhHH", data
    )
    return {
        "timestamp_ms": timestamp_ms,
        "accel_x": accel_x, "accel_y": accel_y, "accel_z": accel_z,
        "gyro_x": gyro_x, "gyro_y": gyro_y, "gyro_z": gyro_z,
        "temp_raw": temp_raw, "vbat_raw": vbat_raw,
    }


def decode_frame(buf: bytes) -> dict | None:
    if len(buf) < FRAME_SIZE:
        return None
    if buf[:2] != SYNC:
        return None
    length = buf[2]
    if length != PAYLOAD_SIZE or len(buf) < 3 + length + 2:
        return None
    payload = buf[3 : 3 + length]
    crc_received = struct.unpack("<H", buf[3 + length : 3 + length + 2])[0]
    if crc16(payload) != crc_received:
        return None
    return decode_payload(payload)


def find_sync(buf: bytes) -> int:
    """Index of first SYNC, or -1."""
    for i in range(len(buf) - 1):
        if buf[i : i + 2] == SYNC:
            return i
    return -1
