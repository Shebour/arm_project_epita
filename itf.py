import serial


class Board:
    def __init__(self, tty: str):
        self.board = serial.Serial(tty, baudrate=115200, bytesize=8, timeout=60)
        self.buffer_size = 512
        self.iv_size = 16

    def _write(self, s: bytes):
        self.board.write(s)

    def _read(self, size: int):
        return self.board.read(size)

    def _ready(self):
        return self.board.is_open

    def _close(self):
        return self.board.close()

    def send_header(self, header: int, size: int) -> bool:
        self._write(header.to_bytes(1, "little"))
        self._write(size.to_bytes(4, "little"))
        response_header = self._read(2).decode()
        print(f"Header response: {response_header}")
        if response_header != "OK":
            return False
        return True

    def read_file(self, filename: str) -> list[str]:
        ret = None
        with open(filename, "r") as f:
            ret = f.read()
        if ret == None:
            raise Exception(f"Fail reading {filename}")
        return split_str(ret, self.buffer_size)


def write_file(filename: str, data: str):
    with open(filename, "w+") as f:
        f.write(data)


def split_str(msg: str, size: int) -> list[str]:
    return [msg[i : i + size] for i in range(0, len(msg), size)]


def init_board(tty: str) -> Board:
    b = Board(tty)
    if not b._ready():
        raise Exception(f"{tty} is not open ! Exiting...")
    return b


def generate_key(board: Board) -> bool:
    print("Key generation")
    ret = board.send_header(0, 0)
    return ret


def aes_encrypt(board: Board, infile: str, outfile: str) -> bool:
    print("Message encryption")
    failed = False
    c_list = board.read_file(infile)
    padding = board.buffer_size - len(c_list[-1])
    c_list[-1] = c_list[-1].ljust(board.buffer_size, "0")
    encoded_data = []
    if board.send_header(1, len(c_list)) == False:
        return False

    for data in c_list:
        board._write(data.encode())
        ret = board._read(board.buffer_size + board.iv_size).hex()
        if ret[:32] == "454e4352595054494f4e204552524f52":
            print("encode failed")
            failed = True
        encoded_data.append(ret)

    encoded_data = "".join(encoded_data)
    with open(outfile, "w+") as f:
        f.write(str(padding))
        f.write("\n")
        f.write(encoded_data)
    return not failed


def aes_decrypt(board: Board, infile: str, outfile: str) -> bool:
    print("Message decryption")
    failed = False
    padding = None
    content = None
    with open(infile, "r") as f:
        padding = f.readline()
        content = f.read()
    if padding == None and content == None:
        raise Exception(f"Fail reading {infile}")

    c_list = split_str(content, (board.buffer_size + board.iv_size) * 2)
    try:
        bytes.fromhex(c_list[0])
    except:
        print(f"Bad content format in {infile}")
        return False
    decoded_data = []
    if not board.send_header(2, len(c_list)):
        return False

    for data in c_list:
        board._write(bytes.fromhex(data))
        ret = board._read(board.buffer_size)
        check = ret[:16]
        if check.decode() == "DECRYPTION ERROR":
            print("decode failed")
            failed = True
        else:
            ret = ret.decode()
            decoded_data.append(ret)
    decoded_data = "".join(decoded_data)
    write_file(outfile, decoded_data[: -int(padding)])
    return not failed
