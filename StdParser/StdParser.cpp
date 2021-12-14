#include <stdio.h>
#include <assert.h>
#include <ctype.h>


char* SkipSpaceSymbolsLeft(char* ptr)
{
    while (*ptr && isspace((unsigned char)*ptr))
        ptr++;
    return ptr;
}

size_t SkipSpaceSymbolsRight(char* ptr, size_t strLength)
{
    while (strLength > 0 && isspace((unsigned char)ptr[strLength - 1]))
        strLength--;
    return strLength;
}

bool SkipInputString(const char* ptr, size_t count)
{
    while (*ptr && count)
    {
        if (!isspace(*ptr))
        {
            return false;
        }
        ptr++;
        count--;
    }
    return true;
}