import argparse
import os
import re
import struct
import subprocess
import sys


# BlastCorps gzip header
# 0x1f8b      # GZIP magic
# 0x08        # DEFLATE
# 0x08        # FLG.FNAME
# 0x????????  # MTIME, seconds since epoch (1st Jan 1970)
# 0x00        # No extra flags
# 0x03        # Unix OS
# 0x??..??00  # variable length, NUL-terminated, filename


def compress_file(filepath, name=None, time=None, level=6):
    # force use of the gzip that sits along this file
    gzip = os.path.join(os.path.dirname(os.path.realpath(__file__)), "gzip")
    args = [gzip, f"-{level}", "--no-name", "-c", filepath]
    res = subprocess.run(args, capture_output=True)
    if res.returncode != 0:
        return None
    return (b'\x1f\x8b' +
            b'\x08' +
            b'\x08' +
            struct.pack("<i", time if time else 0) +
            b'\x00' +
            b'\x03' +
            (bytes(name, 'utf8') if name else b'') + b'\x00' +
            res.stdout[10:])


def parse_filename(filename):
    # filename.ext.time.whatever
    match = re.match(r'([A-z0-9_]+\.*[A-z0-9_]+)\.([0-9]+)', filename)
    if match:
        return match.group(1), int(match.group(2))
    return None, None


def main(inpath, outfile, name=None, time=None, level=6):
    outfile.write(compress_file(inpath, name=name, time=time, level=level))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('infile', type=argparse.FileType('rb'),
                        help='file to read from')
    parser.add_argument('outfile', type=argparse.FileType('wb'),
                        help='file to write to')
    parser.add_argument('--name', type=str, default=None,
                        help='original filename, default: None')
    parser.add_argument('--time', type=int, default=None,
                        help='original modify time, default: None')
    parser.add_argument('--level', type=int, default=6,
                        help='compression level, default: 6')
    # TODO make --auto mutually exclusive from --name + --time
    parser.add_argument('--auto', action='store_true', default=False,
                        help='get name and time from filename, default: No')
    args = parser.parse_args()

    infile_name = args.infile.name
    if args.auto:
        name, time = parse_filename(infile_name)
        if name is None or time is None:
            print("ERROR: Could not get name + time from %s" % infile_name)
            sys.exit(1)
    else:
        name, time = args.name, args.time

    main(infile_name, args.outfile, name=name, time=time, level=args.level)
