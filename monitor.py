import serial
import numpy as np
import cv2

IMG_COL = 320
IMG_ROW = 180
FRAME_BYTES = (IMG_COL * IMG_ROW) // 2

HEADER_BYTE = 0xFF
HEADER_LEN = 4
HEADER = bytes([HEADER_BYTE] * HEADER_LEN)

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
            raise TimeoutError("Timed out while searching for frame header")

        window += b
        if len(window) > HEADER_LEN:
            window.pop(0)

        if len(window) == HEADER_LEN and bytes(window) == HEADER:
            return

ser = serial.Serial('/dev/ttyACM1', timeout=1)
while True:
    find_frame_header(ser)
    raw = read_exact(ser, FRAME_BYTES)

    data = np.frombuffer(raw, dtype=np.uint8)

    p1 = data & 0x0F
    p2 = (data >> 4) & 0x0F

    pixels = np.empty(data.size * 2, dtype=np.uint8)
    pixels[0::2] = p1
    pixels[1::2] = p2

    img = pixels.reshape((IMG_ROW, IMG_COL)).astype(np.uint8) * 17
    

    img_big = cv2.resize(img, None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST)
    cv2.imshow("frame", img_big)
    if cv2.waitKey(1) == 27:
        break

cv2.destroyAllWindows()
ser.close()