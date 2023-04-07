from typing import Any
import serial


def init_board(tty: str):
    board = serial.Serial(tty, baudrate=115200, bytesize=8, timeout=5)
    if not board.is_open:
        raise Exception(f"{tty} is not open ! Exiting...")
    return board


def generate_key(board: Any):
    header = (0).to_bytes(1, "little")
    length = (0).to_bytes(4, "little")
    board.write(header)
    board.write(length)
    ret = board.read(2).decode("utf-8")
    if ret != "OK":
        print("KOOO")
        return False
    return True


def aes_encrypt(board: Any, infile: str, outfile: str):
    lines = []
    l = ""
    with open(infile, "r") as f:
        lines = f.readlines()
        l = "".join(lines)
    print(l)
    n = 128
    split = [l[i : i + n] for i in range(0, len(l), n)]
    header = (1).to_bytes(1, "little")
    length = len(split).to_bytes(4, "little")
    # board.write(header)
    # board.write(length)
