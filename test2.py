#!/usr/bin/env python3

import sys

from itf import *


def ready():
    print("Press button then Enter")
    sys.stdin.read(1)


b = init_board("/dev/ttyACM0")
ready()
r = aes_decrypt(b, "test/file1.enc", "test/file1.out")
assert r == True

with open("test/file1.in") as f:
    src = f.read()
with open("test/file1.out") as f:
    dst = f.read()
assert src == dst
