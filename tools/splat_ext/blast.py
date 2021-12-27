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
    match blast_type:
        case Blast.BLAST0:
            return "unblast0"
        case _:
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
    decoded = bytearray()

    for pos in range(len(encoded) >> 3):
        decoded.extend(encoded[pos * 8: (pos + 1) * 8])

    # Fill with zeros to encoded length
    zeros_to_fill = len(encoded) - len(decoded)
    zeros = bytearray(zeros_to_fill)
    decoded.extend(zeros)

    return decoded


def decode_blast_generic(encoded: bytes, decode_single_fun, element_size: int,
                         loop_back_and: int, loop_back_shift: int) -> bytes:
    decoded_bytes = bytearray()

    for unpacked in struct.iter_unpack(">H", encoded):
        current = unpacked[0]

        if current & 0x8000 == 0:
            res = decode_single_fun(current)
            decoded_bytes.extend(res)
        else:
            loop_back_length = current & 0x1F
            loop_back_offset = (current & loop_back_and) >> loop_back_shift

            slice_from = len(decoded_bytes) - loop_back_offset
            slice_to = slice_from + loop_back_length * element_size

            decoded_bytes.extend(decoded_bytes[slice_from:slice_to])

    return decoded_bytes


# Based on 802A5AE0 (061320)
def decode_blast1(encoded: bytes) -> bytes:
    def single(current: int) -> bytes:
        t1 = (current & 0xFFC0) << 1
        current &= 0x3F
        current |= t1
        return struct.pack(">H", current)
    return decode_blast_generic(encoded, single, 2, 0x7FFF, 5)


# 802A5B90 (0613D0)
def decode_blast2(encoded: bytes) -> bytes:
    def single(current: int) -> bytes:
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
        return struct.pack(">I", t1)
    return decode_blast_generic(encoded, single, 4, 0x7FE0, 4)


# 802A5A2C (06126C)
def decode_blast3(encoded: bytes) -> bytes:
    def single(current: int) -> bytes:
        part0 = current >> 8
        part0 <<= 1
        part1 = current & 0xFF
        part1 <<= 1

        return struct.pack(">BB", part0, part1)
    return decode_blast_generic(encoded, single, 2, 0x7FFF, 5)


# 802A5C5C (06149C)
def decode_blast4(encoded: bytes, lut: bytes) -> bytes:
    def single(current: int) -> bytes:
        part0 = current >> 8
        t2 = part0 & 0xFE
        lookup_bytes = lut[t2:t2 + 2]
        t2 = struct.unpack(">H", lookup_bytes)[0]
        part0 &= 1
        t2 <<= 1
        part0 |= t2

        part1 = current & 0xFE
        lookup_bytes = lut[part1:part1 + 2]
        part1 = struct.unpack(">H", lookup_bytes)[0]
        current &= 1
        part1 <<= 1
        part1 |= current
        return struct.pack(">HH", part0, part1)
    return decode_blast_generic(encoded, single, 4, 0x7FE0, 4)


# 802A5D34 (061574)
def decode_blast5(encoded: bytes, lut: bytes) -> bytes:
    def single(current: int) -> bytes:
        t1 = current >> 4
        t1 = t1 << 1

        lookup_bytes = lut[t1:t1 + 2]
        t1 = struct.unpack(">H", lookup_bytes)[0]

        current &= 0xF
        current <<= 4
        t2 = t1 & 0x7C00
        t3 = t1 & 0x03E0
        t2 <<= 0x11
        t3 <<= 0xE
        t2 |= t3
        t3 = t1 & 0x1F
        t3 <<= 0xB
        t2 |= t3
        t2 |= current

        return struct.pack(">I", t2)
    return decode_blast_generic(encoded, single, 4, 0x7FE0, 4)


# 802A5958 (061198)
def decode_blast6(encoded: bytes) -> bytes:
    def single(current: int) -> bytes:
        part0 = current >> 8
        t2 = part0 & 0x38
        part0 &= 0x07
        t2 <<= 2
        part0 <<= 1
        part0 |= t2

        part1 = current & 0xFF
        t2 = part1 & 0x38
        part1 &= 0x07
        t2 <<= 2
        part1 <<= 1
        part1 |= t2

        return struct.pack(">BB", part0, part1)
    return decode_blast_generic(encoded, single, 2, 0x7FFF, 5)


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
        case Blast.BLAST6_IA8:
            return decode_blast6(encoded)


def decode_blast_lookup(blast_type: Blast, encoded: bytes, lut: bytes) -> bytes:
    match blast_type:
        case Blast.BLAST4_IA16:
            return decode_blast4(encoded, lut)
        case Blast.BLAST5_RGBA32:
            return decode_blast5(encoded, lut)


class N64SegBlast(N64Segment):
    def get_latest_lut256(self):
        lut_dir_path = options.get_asset_path() / self.dir
        lut_files = list(lut_dir_path.glob("*.lut256.bin"))
        lut_files.sort()
        return lut_files[-1]

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

        match blast_type:
            case Blast.BLAST4_IA16:
                lut_path = options.get_asset_path() / self.dir / "047480.lut128.bin"
                with open(lut_path, 'rb') as f:
                    lut = f.read()
                decoded_bytes = decode_blast_lookup(blast_type, encoded_bytes, lut)
            case Blast.BLAST5_RGBA32:
                # TODO: Figure out proper LUT assignment
                lut_path = options.get_asset_path() / self.dir / "152970.lut256.bin"
                if not lut_path.exists():
                    lut_path = self.get_latest_lut256()
                with open(lut_path, 'rb') as f:
                    lut = f.read()
                decoded_bytes = decode_blast_lookup(blast_type, encoded_bytes, lut)
            case _:
                decoded_bytes = decode_blast(blast_type, encoded_bytes)

        # Write decoded file
        decoded_ext = blast_get_decoded_extension(blast_type)
        decoded_file_path = options.get_asset_path() / self.dir / f"{address}.{decoded_ext}"
        with open(decoded_file_path, 'wb') as f:
            f.write(decoded_bytes)

        if blast_type == Blast.BLAST0:
            return

        # Write PNG
        width = self.yaml[4]
        height = self.yaml[5]

        writer_class = blast_get_png_writer(blast_type)
        png_writer = writer_class.get_writer(width, height)
        png_dir_path = options.get_asset_path() / self.dir / f"blast{self.yaml[3]}"
        png_dir_path.mkdir(exist_ok=True)
        png_file_path = png_dir_path / f"{address}.png"
        with open(png_file_path, "wb") as f:
            match blast_type:
                case Blast.BLAST4_IA16:
                    png_writer.write_array(f, decoded_bytes)
                case _:
                    png_writer.write_array(f, writer_class.parse_image(decoded_bytes, width, height, False, True))
