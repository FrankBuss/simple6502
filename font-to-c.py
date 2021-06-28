#!/usr/bin/env python3

from PIL import Image
import numpy as np
import math
import sys
import argparse

# load image
im = Image.open("font.png")
im = im.convert('RGBA')
p = np.array(im)

x0 = 3
y0 = 6
w = 5.5
h = 5.5
ofsx = 31.875
ofsy = 47.733

# convert to C array
for xc in range(16):
    for yc in range(16):
        for yp in range(7):
            c = 0
            i = 16
            for xp in range(5):
                if xc > 0:
                    x = int((xc - 1) * ofsx + xp * w) + x0
                    y = int(ofsy * yc + yp * h) + y0
                    if x >= 0 and x < im.width and y >= 0 and y < im.height:
                        r, g, b, a = p[y][x]
                        if r < 0xf0:
                            c += i
                i //= 2
            sys.stdout.write("0x%02x, " % c)
        sys.stdout.write("\n")
