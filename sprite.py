import cv2
import numpy as np

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

# go through a given image and generate a byte array of the same format as the one sent by the monitor, size of array is size of sprite

def image_to_bytes(image):
    img_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    rows, cols, _ = img_rgb.shape
    pixels = np.zeros((rows, cols), dtype=np.uint8)
    for k, v in COLOUR_MAP.items():
        mask = np.all(img_rgb == v, axis=-1)
        pixels[mask] = k
    # If cols is odd, pad with 0
    if cols % 2 != 0:
        pixels = np.pad(pixels, ((0,0),(0,1)), mode='constant')
        cols += 1
    packed_pixels = (pixels[:, ::2] << 4) | pixels[:, 1::2]
    return packed_pixels.flatten(), rows, cols


def print_c_array(byte_array, rows, cols, name="sprite"):
    length = len(byte_array)
    print(f"// Sprite dimensions: {rows}x{cols}")
    print(f"const uint8_t {name}[{length}] = {{")
    for i, b in enumerate(byte_array):
        print(f"0x{b:02X},", end="")
        if (i+1) % 16 == 0:
            print()
    print("\n};")

image = cv2.imread("./sprites/jet.png")
byte_array, rows, cols = image_to_bytes(image)
print_c_array(byte_array, rows, cols, name="jet_sprite")
