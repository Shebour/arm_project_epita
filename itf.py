from typing import Any
import serial
from Crypto.Cipher import AES


def init_board(tty: str):
    try:
        board = serial.Serial(tty, baudrate=115200)
    except:
        print(f"Error initialization board: {tty}")
        exit(1)
    return board


def generate_key(board: Any):
    header = b"GENKEY\x04"
    print(header)
    board.write(header)


b = init_board("/dev/ttyACM0")
generate_key(b)
