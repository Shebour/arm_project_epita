#include <stdio.h>

#include "libgcov9_syscall.h"

#define LWFSS_INDICATOR '*'
#define LWFSS_SEPARATOR '<'
#define GCOV_END_OF_STREAM "\4\4\4\4"

#define FILES_MAX 100
static const char *stm32Files[FILES_MAX];
static int fileCounter = 0;

static gcov_dump_fn DUMP_FN = NULL;

static void serial_putc(char c) {
    if (DUMP_FN)
        DUMP_FN(&c, 1);
}

static int serial_printf(const char *format, char ptr) {
    char buff[10];
    if (!DUMP_FN)
        return -1;
    int size = snprintf(buff, 10, format, ptr);
    DUMP_FN(buff, size);
    return size;
}

int _gcov_open_file(const char *name, int flags, int mode) {
    if (fileCounter < FILES_MAX) {
        stm32Files[fileCounter] = name;
        return fileCounter++;
    }
    return -1;
}

void _gcov_end(void) {
    DUMP_FN(GCOV_END_OF_STREAM, 4);
}

int _gcov_write_file(int file, char *ptr, int len) {
    static char last = '\n';
    if (file < fileCounter) {
        const char *fileName = stm32Files[file];
        if (last != '\n') // print to new line
            serial_putc('\n');
        serial_putc(LWFSS_INDICATOR); // Start of file
        while (*fileName != '\0' && fileName < (fileName + 5)) // File name
            serial_putc(*fileName++);
        serial_putc(LWFSS_SEPARATOR); // Separator

        for (int i = 0; i < len; i++) // File content
            serial_printf(" %02X", *ptr++);

        serial_putc('\n'); // end with new line
        last = '\n';
    } else {
        for (int i = 0; i < len; i++) { // Write STD
            serial_putc(*ptr++);
        }
        last = *(ptr - 1);
    }

    return len; // amount of data, without the filename
}

void _gcov_set_dump_fn(gcov_dump_fn fn) {
    DUMP_FN = fn;
}
