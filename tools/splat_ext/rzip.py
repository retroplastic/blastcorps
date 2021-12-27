from segtypes.n64.rgba16 import N64SegRgba16
from segtypes.n64.rgba32 import N64SegRgba32
from segtypes.n64.ia8 import N64SegIa8
from segtypes.n64.ia16 import N64SegIa16
from segtypes.n64.img import N64SegImg
from pathlib import Path
from typing import Optional
from segtypes.n64.segment import N64Segment
from util import options
import subprocess


def get_png_writer(file_type: str) -> N64SegImg:
    match file_type:
        case "rgba16":
            return N64SegRgba16
        case "rgba32":
            return N64SegRgba32
        case "ia8":
            return N64SegIa8
        case "ia16":
            return N64SegIa16
        case _:
            return None


class N64SegRzip(N64Segment):
    def out_path(self) -> Optional[Path]:
        return options.get_asset_path() / self.dir / f"{self.name}.gz"

    def split(self, rom_bytes):
        gz_path = self.out_path()

        # Write compressed
        compressed_bytes = rom_bytes[self.rom_start : self.rom_end]
        with open(gz_path, "wb") as f:
            f.write(compressed_bytes)
        self.log(f"Wrote {self.name} to {gz_path}")

        # Decompressed
        subprocess.call(["gzip", "-d", "-k", "-N", "-f", gz_path])
        """
        import gzip
        # TODO: No way to get file name and timestamp from gzip module??
        decompress_path = options.get_asset_path() / self.dir / f"{self.name}.python.raw"
        decompressed = gzip.decompress(compressed_bytes)
        with open(decompress_path, 'wb') as f:
          f.write(decompressed)
        """

        # Write PNG
        if len(self.yaml) == 6:
            gzip_info = subprocess.check_output(["gzip", "-N", "-l", gz_path])

            decompressed_file_name = gzip_info.decode().splitlines()[-1].split()[-1].split("/")[-1]
            decompressed_file_path = options.get_asset_path() / self.dir / decompressed_file_name

            with open(decompressed_file_path, "rb") as f:
                image_bytes = f.read()

            type_str = self.yaml[3]
            width = self.yaml[4]
            height = self.yaml[5]

            writer_class = get_png_writer(type_str)
            png_writer = writer_class.get_writer(width, height)

            png_name = decompressed_file_name.replace("raw", "png")
            png_file_path = options.get_asset_path() / self.dir / png_name

            with open(png_file_path, "wb") as f:
                match type_str:
                    case "ia16":
                        png_writer.write_array(f, image_bytes)
                    case _:
                        png_writer.write_array(f, writer_class.parse_image(image_bytes, width, height, False, False))

