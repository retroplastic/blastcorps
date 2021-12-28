from segtypes.n64.rgba16 import N64SegRgba16
from segtypes.n64.rgba32 import N64SegRgba32
from segtypes.n64.ia8 import N64SegIa8
from segtypes.n64.ia16 import N64SegIa16
from segtypes.n64.img import N64SegImg
from segtypes.n64.segment import N64Segment
from util import options
import subprocess


def split_segment_bytes(subsegments, decoded_bytes: bytes):
    segment_bytes = []
    separators = []
    for segment in subsegments:
        separators.append(segment[0])
    separators.append(len(decoded_bytes))

    for i in range(len(subsegments)):
        from_slice = separators[i]
        to_slice = separators[i + 1]
        segment_bytes.append(decoded_bytes[from_slice:to_slice])

    return segment_bytes


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
    def write_png(self, type_str: str, width: int, height: int, decompressed_file_name: str, image_bytes: bytes):
        writer_class = get_png_writer(type_str)
        png_writer = writer_class.get_writer(width, height)

        png_name = decompressed_file_name.replace("raw", "png")
        png_dir_path = options.get_asset_path() / self.dir / "png" / "rzip"
        png_dir_path.mkdir(exist_ok=True, parents=True)
        png_file_path = png_dir_path / png_name

        with open(png_file_path, "wb") as f:
            match type_str:
                case "ia16":
                    png_writer.write_array(f, image_bytes)
                case _:
                    png_writer.write_array(f, writer_class.parse_image(image_bytes, width, height, False, False))

    def write_png_segments(self, decompressed_file_name: str, segment_bytes: list[bytes], subsegments):
        assert len(segment_bytes) == len(subsegments)

        for i in range(len(segment_bytes)):

            assert len(subsegments[i]) == 4

            type_str = subsegments[i][1]
            width = subsegments[i][2]
            height = subsegments[i][3]

            writer_class = get_png_writer(type_str)
            png_writer = writer_class.get_writer(width, height)

            png_name = decompressed_file_name.replace("raw", f"{i}.png")
            png_dir_path = options.get_asset_path() / self.dir / "png" / "rzip"
            png_dir_path.mkdir(exist_ok=True, parents=True)
            png_file_path = png_dir_path / png_name

            with open(png_file_path, "wb") as f:
                match type_str:
                    case "ia16":
                        png_writer.write_array(f, segment_bytes[i])
                    case _:
                        png_writer.write_array(f, writer_class.parse_image(segment_bytes[i],
                                                                           width, height, False, False))

    def split(self, rom_bytes):
        split_dir_path = options.get_asset_path() / self.dir / "split"
        split_dir_path.mkdir(exist_ok=True, parents=True)
        gz_file_path = split_dir_path / f"{self.name}.gz"

        # Write compressed
        compressed_bytes = rom_bytes[self.rom_start: self.rom_end]
        with open(gz_file_path, "wb") as f:
            f.write(compressed_bytes)

        # Decompressed
        subprocess.call(["gzip", "-d", "-k", "-N", "-f", gz_file_path])

        # Write PNG
        if len(self.yaml) == 6 or isinstance(self.yaml, dict):
            gzip_info = subprocess.check_output(["gzip", "-N", "-l", gz_file_path])

            decompressed_file_name = gzip_info.decode().splitlines()[-1].split()[-1].split("/")[-1]
            decompressed_file_path = options.get_asset_path() / self.dir / "split" / decompressed_file_name

            with open(decompressed_file_path, "rb") as f:
                image_bytes = f.read()

            if isinstance(self.yaml, list):
                # Single segment
                type_str = self.yaml[3]
                width = self.yaml[4]
                height = self.yaml[5]
                self.write_png(type_str, width, height, decompressed_file_name, image_bytes)
            elif isinstance(self.yaml, dict):
                subsegments = self.yaml["subsegments"]
                segment_bytes = split_segment_bytes(subsegments, image_bytes)
                self.write_png_segments(decompressed_file_name, segment_bytes, subsegments)
