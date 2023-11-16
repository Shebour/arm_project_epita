import serial
import struct

GCOV_END_OF_STREAM = b'\4\4\4\4'

def init_board(tty: str):
    s = serial.Serial(tty, baudrate=115200)
    return s

if __name__ == "__main__":
    board = init_board("/dev/ttyUSB0")
    s = "gcov_dump"
    board.write(bytes(s, 'ascii'))
    file = open("inputFile.txt", "wb")

    count_end = 0
    """while True:
        try:
            read_char = board.read(1)
            if read_char == '\4':
            
            file.write(read_char)
        except Exception as e:
            file.close()
    """
    content = board.read_until(expected=GCOV_END_OF_STREAM)
    file.write(content[:-4])
