#ifndef STRING_LIBRARY_H
#define STRING_LIBRARY_H


#include <stdio.h>


struct String
{
    char*  ptr;
    size_t length;
};

struct Text
{
    char*   buffer;
    size_t  bufferSize;
    size_t  stringsCount;
    String* strings;
};

bool ReadFile(Text* text, FILE* file);

void ParseFileToLines(Text* text);

void WriteTextToFile(Text* text, const char* fileName);

size_t GetFileSize(FILE* file);

void TextDestructor(Text* text);


#endif
