import argparse
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
    0xF: (0, 0, 0),         # reserved for header (do not emit)
}

# Exclude the reserved 0xF so black maps to 0x0 only.
COLOR_TO_INDEX = {v: k for k, v in COLOUR_MAP.items() if k != 0xF}


def sanitize_name(name: str) -> str:
    base = os.path.splitext(os.path.basename(name))[0]
    cleaned = re.sub(r"[^A-Za-z0-9_]", "_", base)
    return cleaned


def image_to_bytes(image: np.ndarray):
    has_alpha = image.shape[2] == 4
    img_rgb = cv2.cvtColor(
        image,
        cv2.COLOR_BGRA2RGB if has_alpha else cv2.COLOR_BGR2RGB,
    )
    rows, cols, _ = img_rgb.shape

    pixels = np.full((rows, cols), fill_value=0x0, dtype=np.uint8)
    matched = np.zeros((rows, cols), dtype=bool)

    if has_alpha:
        alpha = image[:, :, 3]
        transparent_mask = alpha == 0
        pixels[transparent_mask] = 0x0
        matched[transparent_mask] = True

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
    # Recover original (unpadded) width; odd widths are padded by one column.
    unpadded_cols = cols if cols % 2 == 0 else cols - 1
    packed_cols = cols // 2

    print(f"// Palette (index: R, G, B)")
    for idx, (r, g, b) in enumerate(palette):
        print(f"// {idx:2d}: ({r}, {g}, {b})")

    print(f"#define {macro_prefix}_WIDTH {unpadded_cols}")
    print(f"#define {macro_prefix}_WIDTH_PACKED {packed_cols}")
    print(f"#define {macro_prefix}_HEIGHT {rows}")

    print(f"static const uint8_t {name}[{length}] = {{")
    for i, b in enumerate(byte_array):
        print(f"0x{b:02X},", end="")
        if (i + 1) % 16 == 0:
            print()
    print("\n};")


def slice_horizontal_spritesheet(image: np.ndarray, tile_size: int = 16):
    """Split a (16 * n) x 16 horizontal strip into a list of tiles."""
    rows, cols, _ = image.shape
    if rows != tile_size:
        raise ValueError(
            f"Spritesheet height must be {tile_size}, got {rows}"
        )
    if cols % tile_size != 0:
        raise ValueError(
            f"Spritesheet width must be a multiple of {tile_size}, got {cols}"
        )
    tiles = []
    for idx in range(cols // tile_size):
        left = idx * tile_size
        right = left + tile_size
        tiles.append(image[:, left:right, :])
    return tiles


def print_c_spritesheet_output(byte_arrays, rows, cols, palette, name="sheet"):
    macro_prefix = sanitize_name(name).upper()
    count = len(byte_arrays)
    if count == 0:
        raise ValueError("No sprites to emit")

    sprite_length = len(byte_arrays[0])
    if any(len(arr) != sprite_length for arr in byte_arrays):
        raise ValueError("All sprites in the sheet must have the same dimensions")

    unpadded_cols = cols if cols % 2 == 0 else cols - 1
    packed_cols = cols // 2

    print(f"// Palette (index: R, G, B)")
    for idx, (r, g, b) in enumerate(palette):
        print(f"// {idx:2d}: ({r}, {g}, {b})")

    print(f"#define {macro_prefix}_SPRITE_WIDTH {unpadded_cols}")
    print(f"#define {macro_prefix}_SPRITE_WIDTH_PACKED {packed_cols}")
    print(f"#define {macro_prefix}_SPRITE_HEIGHT {rows}")
    print(f"#define {macro_prefix}_SPRITE_COUNT {count}")
    print(f"#define {macro_prefix}_SPRITE_SIZE {sprite_length}")

    print(f"static const uint8_t {name}[{count}][{sprite_length}] = {{")
    for idx, arr in enumerate(byte_arrays):
        print(f"    // sprite {idx}")
        print("    {", end="")
        for i, b in enumerate(arr):
            prefix = "\n     " if (i % 16 == 0 and i != 0) else " "
            print(f"{prefix}0x{b:02X},", end="")
        print("\n    },")
    print("};")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convert sprites to packed C arrays")
    parser.add_argument("image_path", help="Path to a PNG sprite or spritesheet")
    parser.add_argument(
        "--sheet",
        action="store_true",
        help="Treat the input as a horizontal spritesheet of 16x16 tiles",
    )
    args = parser.parse_args()

    image_path = args.image_path
    image = cv2.imread(image_path, cv2.IMREAD_UNCHANGED)
    if image is None:
        print(f"Failed to read image: {image_path}", file=sys.stderr)
        sys.exit(1)

    is_sheet = args.sheet
    if not is_sheet:
        rows, cols, _ = image.shape
        is_sheet = rows == 16 and cols % 16 == 0 and cols > 16

    sprite_name = sanitize_name(image_path)

    if is_sheet:
        tiles = slice_horizontal_spritesheet(image, tile_size=16)
        byte_arrays = []
        rows = cols = None
        palette = None
        for tile in tiles:
            byte_array, t_rows, t_cols, t_palette = image_to_bytes(tile)
            byte_arrays.append(byte_array)
            rows = t_rows
            cols = t_cols
            palette = t_palette
        print_c_spritesheet_output(
            byte_arrays, rows, cols, palette, name=sprite_name
        )
    else:
        byte_array, rows, cols, palette = image_to_bytes(image)
        print_c_output(byte_array, rows, cols, palette, name=sprite_name)
