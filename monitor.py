import serial
import numpy as np
import cv2
import sys
import time
import struct

IMG_COL = 180
IMG_ROW = 320
FRAME_BYTES = (IMG_COL * IMG_ROW) // 2

HEADER_BYTE = 0xFF
HEADER_LEN = 4
HEADER = bytes([HEADER_BYTE] * HEADER_LEN)
SCALE = 2

COLOUR_MAP = {
    0x0: (0, 0, 0),         # black / background
    0x1: (32, 163, 248),    # sky blue
    0x2: (40, 24, 19),      # dark leather
    0x3: (104, 222, 238),   # aqua highlight
    0x4: (111, 41, 174),    # violet
    0x5: (114, 95, 101),    # muted mauve
    0x6: (129, 55, 55),     # deep brown-red
    0x7: (158, 19, 40),     # crimson
    0x8: (182, 166, 150),   # warm gray
    0x9: (201, 87, 209),    # orchid
    0xA: (230, 115, 0),     # amber
    0xB: (242, 241, 240),   # off-white
    0xC: (255, 70, 70),     # bright red
    0xD: (255, 196, 31),    # gold
    0xE: (0, 0, 0),         # unused / fallback
    0xF: (0, 0, 0),         # reserved for header
}

palette = np.zeros((16, 3), dtype=np.uint8)
for k, v in COLOUR_MAP.items():
    palette[k] = v

def read_exact(port, n):
    buf = bytearray()
    while len(buf) < n:
        chunk = port.read(n - len(buf))
        if not chunk:
            raise TimeoutError("Timed out while reading frame")
        buf.extend(chunk)
    return bytes(buf)


def find_frame_header(port):
    window = bytearray()

    while True:
        b = port.read(1)
        if not b:
            raise TimeoutError("Timed out while searching for header")

        window += b

        if len(window) > HEADER_LEN:
            window.pop(0)

        if len(window) == HEADER_LEN and bytes(window) == HEADER:
            return

if len(sys.argv) < 2:
    print("Usage: python monitor.py <serial_port>")
    sys.exit(1)
port = sys.argv[1]
ser = serial.Serial(port, timeout=1)
cv2.namedWindow("Game", cv2.WINDOW_NORMAL)


last_time = time.perf_counter()
fps = 0.0
while True:
    find_frame_header(ser)
    score_bytes = read_exact(ser, 2)
    score = struct.unpack('<H', score_bytes)[0]
    raw = read_exact(ser, FRAME_BYTES)

    data = np.frombuffer(raw, dtype=np.uint8)

    p1 = data & 0x0F
    p2 = (data >> 4) & 0x0F

    pixels = np.empty(data.size * 2, dtype=np.uint8)
    pixels[0::2] = p1
    pixels[1::2] = p2

    img = pixels.reshape((IMG_ROW, IMG_COL))
    img_rgb = palette[img]
    img_rgb = cv2.cvtColor(img_rgb, cv2.COLOR_RGB2BGR)

    now = time.perf_counter()
    dt = now - last_time
    if dt > 0:
        inst_fps = 1.0 / dt
        fps = inst_fps if fps == 0.0 else (0.9 * fps + 0.1 * inst_fps)
    last_time = now

    # cv2.putText(
    #     img_rgb,
    #     f"FPS: {fps:.1f}",
    #     (6, 18),
    #     cv2.FONT_HERSHEY_SIMPLEX,
    #     0.5,
    #     (255, 255, 255),
    #     1,
    #     cv2.LINE_AA,
    # )
    score_text = f"{score}"
    text_scale = 0.25
    text_thickness = 1
    (text_w, text_h), _ = cv2.getTextSize(score_text, cv2.FONT_HERSHEY_SIMPLEX, text_scale, text_thickness)
    text_x = max(0, IMG_COL - text_w - 6)
    text_y = 6 + text_h

    cv2.putText(
        img_rgb,
        score_text,
        (text_x, text_y),
        cv2.FONT_HERSHEY_SIMPLEX,
        text_scale,
        (255, 255, 255),
        text_thickness,
        cv2.LINE_AA,
    )

    cv2.resizeWindow("Game", IMG_COL * SCALE, IMG_ROW * SCALE)
    cv2.imshow("Game", img_rgb)

    if cv2.waitKey(1) == 27:
        break

cv2.destroyAllWindows()
ser.close()