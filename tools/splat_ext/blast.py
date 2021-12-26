from segtypes.n64.segment import N64Segment
from segtypes.n64.rgba16 import N64SegRgba16
from segtypes.n64.rgba32 import N64SegRgba32
from segtypes.n64.ia8 import N64SegIa8
from segtypes.n64.ia16 import N64SegIa16
from segtypes.n64.img import N64SegImg
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
    return "%s%d" % (blast_get_format(blast_type), blast_get_depth(blast_type))


def blast_get_png_writer(blast_type: Blast) -> N64SegImg:
    match blast_type:
        case Blast.BLAST1_RGBA16:
            return N64SegRgba16
        case (Blast.BLAST2_RGBA32 | Blast.BLAST5_RGBA32):
            return N64SegRgba32
        case (Blast.BLAST3_IA8 | Blast.BLAST6_IA8):
            return N64SegIa8
        case Blast.BLAST4_IA16:
            return N64SegIa16
        case _:
            return None


# 802A5E10 (061650)
# just a memcpy from a0 to a3
def decode_blast0(encoded: bytes) -> bytes:
    # TODO
    return encoded


# Based on 802A5AE0 (061320)
def decode_blast1(encoded: bytes) -> bytes:
    result_ints = []

    for unpacked in struct.iter_unpack(">H", encoded):
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


# 802A5B90 (0613D0)
def decode_blast2(encoded: bytes) -> bytes:
    result_ints = []

    for unpacked in struct.iter_unpack(">H", encoded):
        current = unpacked[0]

        if current & 0x8000 == 0:
            t1 = current & 0x7800
            t2 = current & 0x0780
            t1 <<= 0x11
            t2 <<= 0xD
            t1 |= t2
            t2 = current & 0x78
            t2 <<= 0x9
            t1 |= t2
            t2 = current & 0x7
            t2 <<= 0x5
            t1 |= t2
            result_ints.append(t1)
        else:
            loop_back_length = current & 0x1F
            loop_back_offset = (current & 0x7FE0) >> 4

            slice_from = len(result_ints) - int(loop_back_offset / 4)
            slice_to = slice_from + loop_back_length
            result_ints.extend(result_ints[slice_from:slice_to])

    decoded_bytes = bytearray()
    for result_int in result_ints:
        b = struct.pack(">I", result_int)
        decoded_bytes.extend(b)

    return decoded_bytes


# 802A5A2C (06126C)
def decode_blast3(encoded: bytes) -> bytes:
    # TODO
    return encoded


# 802A5C5C (06149C)
def decode_blast4(encoded: bytes, lut) -> bytes:
    # TODO
    return encoded


# 802A5D34 (061574)
def decode_blast5(encoded: bytes, lut) -> bytes:
    # TODO
    return encoded


# 802A5958 (061198)
def decode_blast6(encoded: bytes) -> bytes:
    # TODO
    return encoded


def decode_blast(blast_type: Blast, encoded: bytes) -> bytes:
    match blast_type:
        case Blast.BLAST0:
            return decode_blast0(encoded)
        case Blast.BLAST1_RGBA16:
            return decode_blast1(encoded)
        case Blast.BLAST2_RGBA32:
            return decode_blast2(encoded)
        case Blast.BLAST3_IA8:
            return decode_blast3(encoded)
        case Blast.BLAST4_IA16:
            return decode_blast4(encoded, None)
        case Blast.BLAST5_RGBA32:
            return decode_blast5(encoded, None)
        case Blast.BLAST6_IA8:
            return decode_blast6(encoded)


class N64SegBlast(N64Segment):
    def split(self, rom_bytes):
        address = "%06X" % self.yaml[0]

        # Write encoded file
        encoded_ext = "blast%d" % self.yaml[3]
        encoded_file_path = options.get_asset_path() / self.dir / f"{address}.{encoded_ext}"
        encoded_bytes = rom_bytes[self.rom_start: self.rom_end]
        with open(encoded_file_path, 'wb') as f:
            f.write(encoded_bytes)

        # Decode
        blast_type = Blast(self.yaml[3])
        decoded_bytes = decode_blast(blast_type, encoded_bytes)

        # Write decoded file
        decoded_ext = blast_get_decoded_extension(blast_type)
        decoded_file_path = options.get_asset_path() / self.dir / f"{address}.{decoded_ext}"
        with open(decoded_file_path, 'wb') as f:
            f.write(decoded_bytes)

        # Write PNG
        width = self.yaml[5]
        height = self.yaml[6]

        writer_class = blast_get_png_writer(blast_type)
        png_writer = writer_class.get_writer(width, height)
        png_file_path = options.get_asset_path() / self.dir / f"{address}.png"
        with open(png_file_path, "wb") as f:
            png_writer.write_array(f, writer_class.parse_image(decoded_bytes, width, height, False, True))
