#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Stack.h"
#include "StackProtection.h"


int64_t CalculateCRC(const char *buf, size_t len)
{
    assert(buf);

    static bool inited = false;
    static uint64_t crc_table[256] = {0};
    uint64_t crc = 0;

    if (!inited)
    {
        inited = true;
        for (int i = 0; i < 256; i++)
        {
            crc = i;
            for (int j = 0; j < 8; j++)
                crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

            crc_table[i] = crc;
        };
    }

    crc = 0xFFFFFFFFUL;

    while (len--) 
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return (int64_t)(crc ^ 0xFFFFFFFFUL);
};

