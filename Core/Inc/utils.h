#ifndef UTILS_H
#define UTILS_H

#include "stm32f4xx_hal.h"

void Write_Flash(uint8_t data[32]);
void *_memset(void *dest, int val, size_t len);
char *_strncpy(char *dest, const char *src, size_t n);

#endif /* UTILS_H */
