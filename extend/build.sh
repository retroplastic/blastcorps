#!/bin/sh
set -x #echo on

ROM=bc.u.z64

# copy clean ROM
cp bc.u.z64.bak $ROM

# apply patch
# TODO: apply asm patch?
dd if=lut.bin of=$ROM conv=notrunc bs=1 seek=$((0x7FA000))

# gzip code block
gzip -c bin/hd_code_text.raw > bin/hd_code_text.raw.gz

# TODO: assert new gzip size is <= old gzip

# overwrite code
dd if=/dev/zero of=$ROM conv=notrunc bs=1 seek=$((0x787FD0)) count=$((0x7D73B4-0x787FD0))
dd if=bin/hd_code_text.raw.gz of=$ROM conv=notrunc bs=1 seek=$((0x787FD0))

# update checksum
../../sm64tools/n64cksum $ROM