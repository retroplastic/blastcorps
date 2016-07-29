#!/bin/sh
set -x # echo on
set -e # stop shell script if any commands error

LEVEL_DIR=./levels

BASS=~/git/bass/bass/bass

# copy clean ROM and code
for rom in "bc.u.1.0" "bc.u.1.1" "bd.j" "bc.e"
do
  cp $rom.z64.bak $rom.z64
  cp $rom/hd_code_text.raw.bak $rom/hd_code_text.raw
done

# apply source patch to main code
$BASS -d VERSION=VER_U0 -o bc.u.1.0/hd_code_text.raw hd_code_text.asm
$BASS -d VERSION=VER_U1 -o bc.u.1.1/hd_code_text.raw hd_code_text.asm
$BASS -d VERSION=VER_J -o bd.j/hd_code_text.raw hd_code_text.asm
$BASS -d VERSION=VER_E -o bc.e/hd_code_text.raw hd_code_text.asm

# gzip code block
for rom in "bc.u.1.0" "bc.u.1.1" "bd.j" "bc.e"
do
  gzip -c $rom/hd_code_text.raw > $rom/hd_code_text.raw.gz
done

# gzip level blocks
# for f in $LEVEL_DIR/*.raw
# do
#   gzip -c -9 $f > $f.gz
# done

# apply LUT patch and include new code in ROM
# TODO: support all ROMs in blast_corps_rom.asm
for rom in "bc.u.1.0" "bc.u.1.1" "bd.j" "bc.e"
do
  $BASS -o $rom.z64 blast_corps_rom.asm
  # update checksum
  ../../sm64tools/n64cksum $rom.z64
done

# run ROM
#mupen64plus $ROM
