import os
import sys
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from protocol import FRAME_SIZE, SYNC, decode_frame, encode_frame, find_sync


class ProtocolTests(unittest.TestCase):
    def test_fixture_frame_matches_expected_bytes(self) -> None:
        frame = encode_frame(1000, 100, -200, 300, 10, 20, -5, 512, 1024)
        expected_hex = "aa5514e8030000640038ff2c010a001400fbff00020004449d"
        self.assertEqual(frame.hex(), expected_hex)

    def test_round_trip_encode_decode(self) -> None:
        frame = encode_frame(1000, 100, -200, 300, 10, 20, -5, 512, 1024)
        self.assertEqual(len(frame), FRAME_SIZE)
        decoded = decode_frame(frame)
        self.assertIsNotNone(decoded)
        assert decoded is not None
        self.assertEqual(decoded["timestamp_ms"], 1000)
        self.assertEqual(decoded["accel_x"], 100)
        self.assertEqual(decoded["accel_y"], -200)
        self.assertEqual(decoded["accel_z"], 300)
        self.assertEqual(decoded["temp_raw"], 512)
        self.assertEqual(decoded["vbat_raw"], 1024)

    def test_reject_invalid_crc(self) -> None:
        frame = bytearray(encode_frame(1000, 1, 2, 3, 4, 5, 6, 7, 8))
        frame[-1] ^= 0xFF
        self.assertIsNone(decode_frame(bytes(frame)))

    def test_find_sync_position(self) -> None:
        payload = b"\x00\x01\x02" + SYNC + b"\x03\x04"
        self.assertEqual(find_sync(payload), 3)

    def test_find_sync_absent(self) -> None:
        self.assertEqual(find_sync(b"\x00\x01\x02\x03"), -1)


if __name__ == "__main__":
    unittest.main()
