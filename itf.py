from typing import Any
import serial


class Board:
    def __init__(self, tty: str):
        self.board = serial.Serial(tty, baudrate=115200, bytesize=8, timeout=10)
        self.padding_len = -1

    def _write(self, s: bytes):
        self.board.write(s)

    def _read(self, size: int):
        return self.board.read(size)

    def _ready(self):
        return self.board.is_open

    def _close(self):
        return self.board.close()

    def send_header(self, header: int, size: int):
        self._write(header.to_bytes(1, "little"))
        self._write(size.to_bytes(4, "little"))


def init_board(tty: str):
    b = Board(tty)
    if not b._ready():
        raise Exception(f"{tty} is not open ! Exiting...")
    return b


def generate_key(board: Any) -> bool:
    board.send_header(0, 0)
    ret = board._read(2).decode("utf-8")
    if ret != "OK":
        print("Key generation failed")
        return False
    print("Key generated")
    return True


def aes_encrypt(board: Any, infile: str, outfile: str) -> bool:
    content = None
    with open(infile, "r") as f:
        content = f.read()
    if content == None:
        raise Exception(f"Fail reading {infile}")
    n = 512
    content_list = [content[i : i + n] for i in range(0, len(content), n)]

    old_len = len(content_list[-1])
    content_list[-1] = content_list[-1].ljust(512, "0")
    board.padding_len = len(content_list[-1]) - old_len
    encoded_data = []
    board.send_header(1, len(content_list))
    for data in content_list:
        board._write(data.encode())
        encoded_data.append(board._read(512 + 16).hex())

    with open(outfile, "w+") as f:
        for enc in encoded_data:
            print(enc)
            f.write(enc)

    for data, enc_data in zip(content_list, encoded_data):
        if data.encode().hex() == enc_data:
            return False
    return True


def aes_decrypt(board: Any, infile: str, outfile: str):
    content = None
    with open(infile, "r") as f:
        content = f.read()
    if content == None:
        raise Exception(f"Fail reading {infile}")
    n = 528
    content_list = [content[i : i + n] for i in range(0, len(content), n)]
    decoded_data = []
    board.send_header(2, len(content_list))
    for data in content_list:
        print(data)
        board._write(bytes.fromhex(data))
        decoded_data.append(board._read(512))
    with open(outfile, "w+") as f:
        for dec in decoded_data:
            print(dec)
            f.write(dec.decode())

    for data, dec_data in zip(content_list, decoded_data):
        if data == dec_data.hex():
            return False
    return True
