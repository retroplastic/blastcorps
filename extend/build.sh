#!/bin/sh
set -x # echo on
set -e # stop shell script if any commands error

LEVEL_DIR=./levels

BASS=~/git/bass/bass/bass

CODE=./bin/hd_code_text.raw
ROM=bc.u.z64

# copy clean ROM and code
cp $ROM.bak $ROM
cp $CODE.bak $CODE

# apply source patch to main code
$BASS -d VERSION=VER_U1 -o $CODE hd_code_text.asm

# gzip code block
gzip -c $CODE > $CODE.gz

# gzip level blocks
for f in $LEVEL_DIR/*.raw
do
  gzip -c -9 $f > $f.gz
done

# apply LUT patch and include new code in ROM
$BASS -o $ROM blast_corps_rom.asm

# update checksum
../../sm64tools/n64cksum $ROM

# run ROM
#mupen64plus $ROM
