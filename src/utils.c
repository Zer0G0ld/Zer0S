#include "utils.h"

void itoa(uint32_t value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    uint32_t tmp_value;

    // Handle 0 explicitly
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    while (value != 0) {
        tmp_value = value % base;
        *ptr++ = (tmp_value < 10) ? (tmp_value + '0') : (tmp_value - 10 + 'A');
        value /= base;
    }

    *ptr-- = '\0';

    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}
