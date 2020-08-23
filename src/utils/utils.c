#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "utils/logger.h"

// https://gist.github.com/ccbrown/9722406
void dumpHex(const void *data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    DEBUG_FUNCTION_LINE("0x%08X (0x0000): ", data);
    for (i = 0; i < size; ++i) {
        WHBLogPrintf("%02X ", ((unsigned char *) data)[i]);
        if (((unsigned char *) data)[i] >= ' ' && ((unsigned char *) data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char *) data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size) {
            WHBLogPrintf(" ");
            if ((i + 1) % 16 == 0) {
                WHBLogPrintf("|  %s \n", ascii);
                if (i + 1 < size) {
                    DEBUG_FUNCTION_LINE("0x%08X (0x%04X); ", data + i + 1, i + 1);
                }
            } else if (i + 1 == size) {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8) {
                    WHBLogPrintf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j) {
                    WHBLogPrintf("   ");
                }
                WHBLogPrintf("|  %s \n", ascii);
            }
        }
    }
}
