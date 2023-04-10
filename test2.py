#!/usr/bin/env python3

import sys

from itf import *


def ready():
    print("Press button then Enter")
    sys.stdin.read(1)


b = init_board("/dev/ttyACM0")
ready()
r = aes_encrypt(b, "test/file.in", "test/file.enc")
assert r == True

with open("test/file.in", "r") as f:
    src = f.read()
with open("test/file.out", "r") as f:
    dst = f.read()
assert src == dst
