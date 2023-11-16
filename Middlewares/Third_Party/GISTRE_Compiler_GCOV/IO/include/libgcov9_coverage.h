#ifndef _GCOV_COVERAGE_H
#define _GCOV_COVERAGE_H

#include "main.h"
#include "GISTRE.libgcov9_conf.h"
/**
 * Set the coverage dump function that will be used to send
 * coverage data.
 */
void gcov_init();

void UART_callback(UART_HandleTypeDef *huart);

#endif // _GCOV_COVERAGE_H
