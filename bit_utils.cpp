#include "bit_utils.h"

int lsb_index(uint64_t n) {
    if (!n) {
        return 64;
    }

    #if __LONG_MAX__  == 2147483647L
        uint32_t index;
        __asm__ __volatile__(
            "bsf %1, %0":
            "=r"(index):
            "rm"(n));
    #else
        uint64_t index;
        __asm__ __volatile__(
            "bsfq %1, %0":
            "=r"(index):
            "rm"(n));
    #endif

    return index;

}

int msb_index(uint64_t n) {
    if (!n) {
        return 64;
    }

    #if __LONG_MAX__  == 2147483647L
        uint32_t index;
        __asm__ __volatile__(
            "bsr %1, %0":
            "=r"(index):
            "rm"(n));
    #else
        uint64_t index;
        __asm__ __volatile__(
            "bsrq %1, %0":
            "=r"(index):
            "rm"(n));
    #endif
    
    return index;
}

int pop_count(uint64_t n) {
    return __builtin_popcount(n);
}
