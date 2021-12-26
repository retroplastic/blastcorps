from segtypes.n64.segment import N64Segment
from util import options
import struct


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

    decoded_bytes = bytearray(b"")
    for result_int in result_ints:
        b = struct.pack(">H", result_int)
        decoded_bytes.extend(b)

    return decoded_bytes


class N64SegBlast(N64Segment):
    def split(self, rom_bytes):
        print(self.yaml)

        decoded_file_path = options.get_asset_path() / self.dir / f"{self.name}.unblast"
        print(decoded_file_path)

        compressed_bytes = rom_bytes[self.rom_start: self.rom_end]

        decoded_bytes = decode_blast0(compressed_bytes)
        with open(decoded_file_path, 'wb') as f:
            f.write(decoded_bytes)
