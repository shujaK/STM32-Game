"""Extract and print all unique RGB colors from an image file."""

from __future__ import annotations

import pathlib
import sys
from typing import Iterable, List, Tuple

from PIL import Image


Color = Tuple[int, int, int]


def extract_unique_colors(image_path: pathlib.Path) -> List[Color]:
	"""Return a sorted list of unique RGB tuples found in the image."""
	with Image.open(image_path) as img:
		rgb_image = img.convert("RGB")  # Normalize to RGB
		unique = set(rgb_image.getdata())
	return sorted(unique)


def save_color_strip(colors: List[Color], output_path: pathlib.Path) -> None:
	"""Save colors as an n x 1 strip image."""
	if not colors:
		raise ValueError("No colors to export")

	strip = Image.new("RGB", (len(colors), 1))
	strip.putdata(colors)
	strip.save(output_path)


def print_colors(colors: Iterable[Color]) -> None:
	for idx, (r, g, b) in enumerate(colors, start=1):
		print(f"{idx:4d}: ({r}, {g}, {b})")


def main(argv: list[str]) -> int:
	if len(argv) not in (2, 3):
		print("Usage: python extract.py <image_path> [output_strip.png]", file=sys.stderr)
		return 1

	image_path = pathlib.Path(argv[1])
	if not image_path.is_file():
		print(f"Error: file not found - {image_path}", file=sys.stderr)
		return 1

	output_path = pathlib.Path(argv[2]) if len(argv) == 3 else image_path.with_suffix("")
	if len(argv) == 2:
		output_path = output_path.with_name(output_path.name + "_colors.png")
	elif output_path.suffix.lower() not in {".png", ".bmp", ".jpg", ".jpeg"}:
		output_path = output_path.with_suffix(".png")

	colors = extract_unique_colors(image_path)
	print_colors(colors)
	print(f"Total unique colors: {len(colors)}")

	save_color_strip(colors, output_path)
	print(f"Saved color strip to {output_path}")
	return 0


if __name__ == "__main__":
	raise SystemExit(main(sys.argv))
