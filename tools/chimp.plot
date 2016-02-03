set term png size 1024,1024
set output "chimp.png"
#set term dumb
set xrange [3200:700]
set yrange [1700:5200]
set title "Simian Acres / chimp.raw"
#unset xtics
#unset ytics
set key box 3
set grid

set style line 1 lc rgb 'orange' lt 1 lw 2 pt 3 ps 1.5
set style line 2 lc rgb 'black'  lt 1 lw 2 pt 5 ps 1.5
set style line 3 lc rgb 'blue'   lt 1 lw 2 pt 7 ps 1.5
set style line 4 lc rgb 'red'    lt 1 lw 2 pt 7 ps 2.0
set style line 5 lc rgb 'brown'  lt 1 lw 2 pt 5 ps 2.0
set style line 6 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5
set style line 7 lc rgb '#FF6060' lt 1 lw 2 pt 7 ps 1.5

# level bounds from 0x4C
#set object 1 rect from 0x009E, 0x011B to 0x10B9, 0x1FA8 fc rgb "green" fs pattern 1 bo -1

# bounding boxes from 0x40
#set object 1 rect from 0x00D2, 0x041A to 0x1072, 0x1B8A fc rgb "cyan" fs pattern 1 bo -1
#set object 2 rect from 0x00D2, 0x041A to 0x1072, 0x1B8A fc rgb "red" fs pattern 4 bo -1

#set object 3 rect from 0x07DA, 0x0ABE to 0x096A, 0x0B86 fc rgb "red" fs pattern 1 bo -1
#set object 4 rect from 0x064A, 0x0ABE to 0x0776, 0x0C4E fc rgb "red" fs pattern 1 bo -1
#set object 5 rect from 0x0456, 0x0ABE to 0x051E, 0x0BEA fc rgb "red" fs pattern 1 bo -1
#set object 6 rect from 0x07DA, 0x0C4E to 0x083E, 0x0D16 fc rgb "red" fs pattern 1 bo -1
#set object 7 rect from 0x064A, 0x0CB2 to 0x0712, 0x0E42 fc rgb "red" fs pattern 1 bo -1
#set object 8 rect from 0x0712, 0x0DDE to 0x096A, 0x0EA6 fc rgb "red" fs pattern 1 bo -1
#set object 9 rect from 0x0776, 0x0D16 to 0x083E, 0x0DDE fc rgb "red" fs pattern 1 bo -1
#set object 10 rect from 0x07DA, 0x0C4E to 0x08A2, 0x0D16 fc rgb "red" fs pattern 1 bo -1
#set object 11 rect from 0x05E6, 0x0EA6 to 0x06AE, 0x1036 fc rgb "red" fs pattern 1 bo -1
#set object 12 rect from 0x051E, 0x1036 to 0x064A, 0x10FE fc rgb "red" fs pattern 1 bo -1
#set object 13 rect from 0x0712, 0x0F6E to 0x08A2, 0x0FD2 fc rgb "red" fs pattern 1 bo -1
#set object 14 rect from 0x096A, 0x0E42 to 0x0A32, 0x0F0A fc rgb "red" fs pattern 1 bo -1
#set object 15 rect from 0x09CE, 0x0F6E to 0x0A96, 0x109A fc rgb "red" fs pattern 1 bo -1
#set object 16 rect from 0x0906, 0x109A to 0x09CE, 0x1162 fc rgb "red" fs pattern 1 bo -1
#set object 17 rect from 0x083E, 0x10FE to 0x0906, 0x122A fc rgb "red" fs pattern 1 bo -1
#set object 18 rect from 0x0712, 0x1162 to 0x07DA, 0x12F2 fc rgb "red" fs pattern 1 bo -1
#set object 19 rect from 0x08A2, 0x0F6E to 0x096A, 0x1036 fc rgb "red" fs pattern 1 bo -1
#set object 20 rect from 0x0A32, 0x0DDE to 0x0AFA, 0x0EA6 fc rgb "red" fs pattern 1 bo -1
#set object 21 rect from 0x0712, 0x109A to 0x083E, 0x10FE fc rgb "red" fs pattern 1 bo -1
#set object 22 rect from 0x0712, 0x0FD2 to 0x0776, 0x109A fc rgb "red" fs pattern 1 bo -1
#set object 23 rect from 0x09CE, 0x0CB2 to 0x0A96, 0x0DDE fc rgb "red" fs pattern 1 bo -1

#set object 24 rect from 0x0C8A, 0x041A to 0x1072, 0x1B8A fc rgb "yellow" fs pattern 2 bo -1
#set object 25 rect from 0x0AFA, 0x041A to 0x0C8A, 0x0C4E fc rgb "yellow" fs pattern 2 bo -1

# bounding boxes from 0x44
#set object 26 rect from 0x00D2, 0x0AD5 to 0x1072, 0x1B8A fc rgb "black" fs pattern 4 bo -1
#set object 27 rect from 0x01FE, 0x0B22 to 0x051E, 0x18CE fc rgb "black" fs pattern 4 bo -1
#set object 28 rect from 0x051E, 0x128E to 0x0776, 0x18CE fc rgb "black" fs pattern 2 bo -1
#set object 29 rect from 0x08A2, 0x0CB2 to 0x0E1A, 0x18CE fc rgb "black" fs pattern 2 bo -1
#set object 30 rect from 0x00D2, 0x0305 to 0x1072, 0x0AD5 fc rgb "black" fs pattern 2 bo -1

# RDUs: offsets 0x34-0x38: 01B6B4-01B90C
# TNTs: offsets 0x38-0x3C: 01B90C-01B924
# to read from chimp.raw: skip=0x01B6B4 
plot 'chimp.raw.34rdus.bin' binary endian=big format="%int16%int16%int16" using 1:3 with points ls 1 title "RDUs", \
     'chimp.raw.38tnts.bin' binary endian=big format="%int16%int16%int16%int16%int16%int16" using 1:3 with points ls 2 title "TNT", \
     'chimp.raw.50vehicles.bin' binary endian=big format="%uchar%int16%int16%int16%int16" using 2:4 with points ls 3 title "Vehicles", \
     'chimp.raw.54.txt' using 1:2:3:4 with vectors filled ls 4 title "Missile", \
     'chimp.raw.5C.bin' binary endian=big format="%int16%int16%int16%int16%uchar%uchar%int16%int16" using 1:3 with points ls 5 title "Structures", \
     'chimp.raw.68.1.txt' using 1:3 with lines ls 6 title "Train Platform", \
     'chimp.raw.68.2.txt' using 1:3 with lines ls 6 notitle, \
     'chimp.raw.68.3.txt' using 1:3 with lines ls 6 notitle, \
     'chimp.raw.68.4.txt' using 1:3 with lines ls 6 notitle, \
     'chimp.raw.68.01.txt' using 1:2 with lines ls 6 notitle, \
     'chimp.raw.68.02.txt' using 1:2 with lines ls 6 notitle


