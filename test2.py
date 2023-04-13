#!/usr/bin/env python3

import sys

from itf import *


def ready():
    print("Press button then Enter")
    sys.stdin.read(1)


def compare_file(infile, encfile, outfile):
    with open(infile, "r") as f:
        src = f.read()
    with open(encfile, "r") as f:
        enc = f.read()
    with open(outfile, "r") as f:
        dst = f.read()
    if src != dst:
        print(f"error:{infile}, {outfile}, {encfile}")


b = init_board("/dev/ttyACM0")

ready()
# Appui bouton
r = generate_key(b)
assert r == True
ready()
# Appui bouton
r = aes_encrypt(b, "test/file.in", "test/file.enc")
assert r == True
ready()
r = aes_encrypt(b, "test/file1.in", "test/file1.enc")
assert r == True
ready()
r = aes_encrypt(b, "test/file2.in", "test/file2.enc")
assert r == True
print("reset")
ready()
# Appui bouton
r = aes_decrypt(b, "test/file.enc", "test/file.out")
assert r == True
compare_file("test/file.in", "test/file.enc", "test/file.out")
ready()
r = aes_decrypt(b, "test/file1.enc", "test/file1.out")
assert r == True
compare_file("test/file1.in", "test/file1.enc", "test/file1.out")
ready()
r = aes_decrypt(b, "test/file2.enc", "test/file2.out")
assert r == True
compare_file("test/file2.in", "test/file2.enc", "test/file2.out")
