#include "utils.h"

void Write_Flash(uint8_t data[32])
{
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR
                         | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
  FLASH_Erase_Sector(FLASH_SECTOR_1, VOLTAGE_RANGE_3);
  uint32_t start_address = 0x8004000;
  for (int i = 0; i < 32; i++)
  {
    HAL_FLASH_Program(TYPEPROGRAM_BYTE, start_address, data[i]);
    start_address += 1;
  }
  HAL_FLASH_Lock();
}
void *_memset(void *dest, int val, size_t len)
{
  unsigned char *ptr = (unsigned char *)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}
char *_strncpy(char *dest, const char *src, size_t n)
{
  if (n == 0)
    return dest;
  char *d = dest;
  const char *s = src;
  while (n > 0)
  {
    *d++ = *s++;
    n--;
  }
  return dest;
}
