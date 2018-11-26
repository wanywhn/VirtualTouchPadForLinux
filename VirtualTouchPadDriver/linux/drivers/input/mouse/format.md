Config Packet
byte 0:

bit   7   6   5   4   3   2   1   0
      .   .   .   .   0   1   R   L
      resx


byte 1:

bit   7   6   5   4   3   2   1   0
      .   .   . ft4 ft3 ft2 ft1 ft0

        resy

byte 2:

    maxx mm 0..7

byte 3:

bit   7   6   5   4   3   2   1   0
      .   .   .   0   0   0   1   1

      constant bits

byte 4:

bit   7   6   5   4   3   2   1   0
      .   .   .   .   .   .   .   .

    maxy mm 0..7

byte 5:
bit   7   6   5   4   3   2   1   0
      .   .   .   .   .   .   .   .

      maxx  8..11     maxy 8..11
