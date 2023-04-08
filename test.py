#!/usr/bin/env python3
import time

from itf import *

b = init_board("/dev/ttyACM0")
r = generate_key(b)
assert r == False
print("Sleeping 5sec... (Press button)")
time.sleep(5)
# Appui bouton
r = generate_key(b)
assert r == True
print("Sleeping 5sec... (Press button)")
time.sleep(5)
# Appui bouton
r = aes_encrypt(b, "test/file.in", "test/file.enc")
assert r == True
print("Sleeping 5sec... (Press button)")
time.sleep(5)
# Appui bouton
r = aes_decrypt(b, "test/file.enc", "test/file.out")
assert r == True
with open("test/file.in", "r") as f:
    src = f.read()
with open("test/file.enc", "r") as f:
    enc = f.read()
assert enc != src
with open("test/file.out", "r") as f:
    dst = f.read()
assert src == dst
