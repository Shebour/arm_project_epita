import serial

GCOV_END_OF_STREAM = b'\4\4\4\4'

def init_board(tty: str):
    s = serial.Serial(tty, baudrate=115200)
    return s

if __name__ == "__main__":
    board = init_board("/dev/ttyUSB0")

    board.write(bytes('gcov_dump', 'ascii'))
    file = open("inputFile.txt", "wb")

    content = board.read_until(expected=GCOV_END_OF_STREAM)
    file.write(content[:-4])

    file.close()
