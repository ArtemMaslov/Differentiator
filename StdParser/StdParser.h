#ifndef STD_PARSER_H_
#define STD_PARSER_H_


char* SkipSpaceSymbolsLeft(char* ptr);

bool SkipInputString(const char* ptr, size_t count);

size_t SkipSpaceSymbolsRight(char* ptr, size_t strLength);


#endif
