#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "StringLibrary.h"

bool ReadFile(Text* text, FILE* file)
{
    assert(text);
    //assert(file);

    if (!file)
        return false;

    text->buffer       = nullptr;
    text->bufferSize   = 0;
    text->strings      = nullptr;
    text->stringsCount = 0;

    size_t fileSize = GetFileSize(file);

    text->buffer = (char*)calloc(fileSize + 1, sizeof(char));// Прибавляем 1, чтобы в конце файла был нуль-терминатор

    if (text->buffer)
    {
        size_t readed = fread(text->buffer, sizeof(char), fileSize, file);
        text->bufferSize = readed + 1;

        if (readed == 0)
        {
            puts("Ошибка чтения файла.");
            return false;
        }
        text->buffer[readed] = '\0';
    }
    else
    {
        puts("Ошибка выделения памяти.");
        return false;
    }

    fclose(file);
    return true;
}

size_t GetFileSize(FILE* file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    return fileSize;
}

void TextDestructor(Text* text)
{
    if (text)
    {
        free(text->buffer);
        free(text->strings);
    }
}
