#ifndef __BIT_UTILS__
#define __BIT_UTILS__

#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int lsb_index(uint64_t n);
int msb_index(uint64_t n);
int pop_count(uint64_t n);

#endif // __BIT_UTILS__