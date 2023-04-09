#!/usr/bin/env python3
import sys

from itf import *


def ready():
    print("Press button then any key")
    sys.stdin.read(1)


b = init_board("/dev/ttyACM0")
"""
r = generate_key(b)
assert r == False
"""
ready()
# Appui bouton
r = generate_key(b)
assert r == True
ready()
# Appui bouton
r = aes_encrypt(b, "test/file1.in", "test/file1.enc")
assert r == True
ready()
# Appui bouton
r = aes_decrypt(b, "test/file1.enc", "test/file1.out")
assert r == True
with open("test/file1.in", "r") as f:
    src = f.read()
with open("test/file1.enc", "r") as f:
    enc = f.read()
assert enc != src
with open("test/file1.out", "r") as f:
    dst = f.read()
assert src == dst
print("Reset board")
ready()
r = aes_decrypt(b, "test/file1.enc", "test/file6.out")
assert r == True
with open("test/file1.in", "r") as f:
    src = f.read()
with open("test/file1.enc", "r") as f:
    enc = f.read()
assert enc != src
with open("test/file6.out", "r") as f:
    dst = f.read()
assert src == dst
