from segtypes.n64.segment import N64Segment
from segtypes.n64.rgba16 import N64SegRgba16
from util import options
import struct
from enum import Enum


class Blast(Enum):
    BLAST0 = 0
    BLAST1_RGBA16 = 1
    BLAST2_RGBA32 = 2
    BLAST3_IA8 = 3
    BLAST4_IA16 = 4
    BLAST5_RGBA32 = 5
    BLAST6_IA8 = 6


def blast_get_depth(blast_type: Blast) -> int:
    match blast_type:
        case (Blast.BLAST3_IA8 | Blast.BLAST6_IA8):
            return 8
        case (Blast.BLAST1_RGBA16 | Blast.BLAST4_IA16):
            return 16
        case (Blast.BLAST2_RGBA32 | Blast.BLAST5_RGBA32):
            return 32
        case _:
            return 0


def blast_get_format(blast_type: Blast) -> str:
    match blast_type:
        case (Blast.BLAST1_RGBA16 | Blast.BLAST2_RGBA32 | Blast.BLAST5_RGBA32):
            return "rgba"
        case (Blast.BLAST3_IA8 | Blast.BLAST4_IA16 | Blast.BLAST6_IA8):
            return "ia"
        case _:
            return ""


def blast_get_decoded_extension(blast_type: Blast) -> str:
    print("Got", blast_type)
    return "%s%d" % (blast_get_format(blast_type), blast_get_depth(blast_type))


def decode_blast0(compressed_bytes):
    result_ints = []

    for unpacked in struct.iter_unpack(">H", compressed_bytes):
        current = unpacked[0]

        if current & 0x8000 == 0:
            t1 = (current & 0xFFC0) << 1
            current &= 0x3F
            current |= t1
            result_ints.append(current)
        else:
            loop_back_length = current & 0x1F
            loop_back_offset = (current & 0x7FFF) >> 5

            slice_from = len(result_ints) - int(loop_back_offset / 2)
            slice_to = slice_from + loop_back_length
            result_ints.extend(result_ints[slice_from:slice_to])

    decoded_bytes = bytearray()
    for result_int in result_ints:
        b = struct.pack(">H", result_int)
        decoded_bytes.extend(b)

    return decoded_bytes


class N64SegBlast(N64Segment):
    def split(self, rom_bytes):
        print(self.yaml)

        address = "%06X" % self.yaml[0]

        blast_type = Blast(self.yaml[3])

        width = self.yaml[5]
        height = self.yaml[6]

        print(address, blast_type)

        # Write encoded file
        encoded_ext = "blast%d" % self.yaml[3]
        encoded_file_path = options.get_asset_path() / self.dir / f"{address}.{encoded_ext}"
        encoded_bytes = rom_bytes[self.rom_start: self.rom_end]
        print("Writing", encoded_file_path)
        with open(encoded_file_path, 'wb') as f:
            f.write(encoded_bytes)

        # Decode and write decoded file
        decoded_ext = blast_get_decoded_extension(blast_type)
        decoded_file_path = options.get_asset_path() / self.dir / f"{address}.{decoded_ext}"
        print("Writing", decoded_file_path)
        decoded_bytes = decode_blast0(encoded_bytes)
        with open(decoded_file_path, 'wb') as f:
            f.write(decoded_bytes)

        writer_class = N64SegRgba16

        png_writer = writer_class.get_writer(width, height)
        png_file_path = options.get_asset_path() / self.dir / f"{address}.png"

        with open(png_file_path, "wb") as f:
            png_writer.write_array(f, writer_class.parse_image(decoded_bytes, width, height, False, True))
