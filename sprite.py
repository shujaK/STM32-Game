import os
import re
import sys

import cv2
import numpy as np


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

COLOR_TO_INDEX = {v: k for k, v in COLOUR_MAP.items()}


def sanitize_name(name: str) -> str:
    base = os.path.splitext(os.path.basename(name))[0]
    cleaned = re.sub(r"[^A-Za-z0-9_]", "_", base)
    return cleaned


def image_to_bytes(image: np.ndarray):
    img_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    rows, cols, _ = img_rgb.shape

    pixels = np.full((rows, cols), fill_value=0x0, dtype=np.uint8)
    matched = np.zeros((rows, cols), dtype=bool)

    for color, idx in COLOR_TO_INDEX.items():
        mask = np.all(img_rgb == color, axis=-1)
        pixels[mask] = idx
        matched |= mask

    if not matched.all():
        bad_locs = np.argwhere(~matched)
        y, x = bad_locs[0]  # first offending pixel
        sample = img_rgb[y, x]
        bad_pixels = img_rgb[~matched]
        unique_bad = np.unique(bad_pixels.reshape(-1, 3), axis=0)
        raise ValueError(
            f"Found colors not in palette at (x={x}, y={y}). "
            f"Example: ({sample[0]}, {sample[1]}, {sample[2]}); "
            f"unique offending colors: {unique_bad.tolist()}"
        )

    padded_cols = cols
    if cols % 2 != 0:
        pixels = np.pad(pixels, ((0, 0), (0, 1)), mode="constant")
        padded_cols += 1

    packed_pixels = (pixels[:, ::2] << 4) | pixels[:, 1::2]
    # Palette order is fixed by COLOUR_MAP keys
    palette = [COLOUR_MAP[i] for i in sorted(COLOUR_MAP.keys())]
    return packed_pixels.flatten(), rows, padded_cols, palette


def print_c_output(byte_array, rows, cols, palette, name="sprite"):
    macro_prefix = sanitize_name(name).upper()
    length = len(byte_array)

    print(f"// Palette (index: R, G, B)")
    for idx, (r, g, b) in enumerate(palette):
        print(f"// {idx:2d}: ({r}, {g}, {b})")

    print(f"#define {macro_prefix}_WIDTH {cols-1}")
    print(f"#define {macro_prefix}_WIDTH_PACKED {cols}")
    print(f"#define {macro_prefix}_HEIGHT {rows}")

    print(f"const uint8_t {name}[{length}] = {{")
    for i, b in enumerate(byte_array):
        print(f"0x{b:02X},", end="")
        if (i + 1) % 16 == 0:
            print()
    print("\n};")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python sprite.py <image.png>", file=sys.stderr)
        sys.exit(1)

    image_path = sys.argv[1]
    image = cv2.imread(image_path)
    if image is None:
        print(f"Failed to read image: {image_path}", file=sys.stderr)
        sys.exit(1)

    byte_array, rows, cols, palette = image_to_bytes(image)
    sprite_name = sanitize_name(image_path)
    print_c_output(byte_array, rows, cols, palette, name=sprite_name)
