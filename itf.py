from typing import Any
import serial
from Crypto.Cipher import AES


def init_board(tty: str):
    try:
        board = serial.Serial(tty, baudrate=115200, bytesize=8, timeout=10)
    except:
        print(f"Error initialization board: {tty}")
        exit(1)
    return board


def generate_key(board: Any):
    header = (0).to_bytes(1, "little")
    length = (0).to_bytes(4, "little")
    board.write(header)
    board.write(length)
    return True


def aes_encrypt(board: Any, infile: str, outfile: str):
    lines = []
    with open(infile, "r") as f:
        lines = f.readlines()
        " ".join(lines)
    length = len(lines).to_bytes(4, "little")
    header = (1).to_bytes(1, "little")
    board.write(header)
    board.write(length)
    size = 0
    s = ""
    while size < len(lines):
        s += board.read(128)
        size += 128
    with open(outfile, "w") as f:
        f.write(s)


b = init_board("/dev/ttyACM0")
generate_key(b)
