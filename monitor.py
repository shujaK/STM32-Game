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
SCALE = 4

COLOUR_MAP = {
    0x0: (0, 0, 0),         # BLACK
    0x1: (255, 255, 255),   # WHITE
    0x2: (0, 0, 255),       # BLUE
    0x3: (0, 255, 0),       # GREEN
    0x4: (0, 255, 255),     # CYAN
    0x5: (255, 0, 0),       # RED
    0x6: (255, 0, 255),     # MAGENTA
    0x7: (165, 42, 42),     # BROWN
    0x8: (211, 211, 211),   # LIGHTGRAY
    0x9: (105, 105, 105),   # DARKGRAY
    0xA: (173, 216, 230),   # LIGHTBLUE
    0xB: (144, 238, 144),   # LIGHTGREEN
    0xC: (224, 255, 255),   # LIGHTCYAN
    0xD: (255, 182, 182),   # LIGHTRED 
    0xE: (255, 160, 255),   # LIGHTMAGENTA
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

    cv2.putText(
        img_rgb,
        f"FPS: {fps:.1f}",
        (6, 18),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.5,
        (255, 255, 255),
        1,
        cv2.LINE_AA,
    )
    cv2.putText(
        img_rgb,
        f"Score: {score}",
        (6, 36),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.5,
        (255, 255, 255),
        1,
        cv2.LINE_AA,
    )

    cv2.resizeWindow("Game", IMG_COL * SCALE, IMG_ROW * SCALE)
    cv2.imshow("Game", img_rgb)

    if cv2.waitKey(1) == 27:
        break

cv2.destroyAllWindows()
ser.close()