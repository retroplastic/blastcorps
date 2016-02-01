#!/bin/sh
set -x #echo on

BASS=~/git/bass/bass/bass

CODE=./bin/hd_code_text.raw
ROM=bc.u.z64

# copy clean ROM
cp $ROM.bak $ROM
cp $CODE.bak $CODE

# apply LUT patch
dd if=lut.bin of=$ROM conv=notrunc bs=1 seek=$((0x7FA000))

# apply source patch
$BASS -o $CODE use_lut.s

# gzip code block
gzip -c $CODE > $CODE.gz

# TODO: assert new gzip size is <= old gzip

# overwrite code
dd if=/dev/zero of=$ROM conv=notrunc bs=1 seek=$((0x787FD0)) count=$((0x7D73B4-0x787FD0))
dd if=$CODE.gz of=$ROM conv=notrunc bs=1 seek=$((0x787FD0))

# update checksum
../../sm64tools/n64cksum $ROM
