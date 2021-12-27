from pathlib import Path
from typing import Optional
from segtypes.n64.segment import N64Segment
from util import options
from util import log
import subprocess
import gzip

class N64SegRzip(N64Segment):
    def out_path(self) -> Optional[Path]:
        return options.get_asset_path() / self.dir / f"{self.name}.gz"

    def split(self, rom_bytes):
        path = self.out_path()
        path.parent.mkdir(parents=True, exist_ok=True)

        if self.rom_end == "auto":
            log.error(f"segment {self.name} needs to know where it ends; add a position marker [0xDEADBEEF] after it")

        compressed_bytes = rom_bytes[self.rom_start : self.rom_end]
        with open(path, "wb") as f:
            f.write(compressed_bytes)
        self.log(f"Wrote {self.name} to {path}")

        subprocess.call(["gzip", "-d", "-k", "-N", "-f", path])

        # TODO: Grab file name from gzip and write proper timestamp
        """
        decompress_path = options.get_asset_path() / self.dir / f"{self.name}.python.raw"
        decompressed = gzip.decompress(compressed_bytes)
        with open(decompress_path, 'wb') as f:
          f.write(decompressed)
        """
