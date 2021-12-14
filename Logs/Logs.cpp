#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <assert.h>

#include "Logs.h"

int TextOffset = 0;

static FILE* logFile = nullptr;

bool LogConstructor(const char* logFileName, const char* caption)
{
    assert(logFileName);
    assert(caption);

    logFile = fopen(logFileName, "w");

    if (!logFile)
    {
        printf("Ошибка открытия файла с логами. FileName = \"%s\"", logFileName);
        return false;
    }

    atexit(LogDestructor);

    time_t rawTime = time(nullptr);
    tm* curTime = localtime(&rawTime);

    const char* dateFormat = "00:00:00 01.01.2021";
    size_t dateLength = strlen(dateFormat) + 1;
    char* date = (char*)calloc(dateLength, sizeof(char));

    if (date)
        strftime(date, dateLength, "%H:%M:%S %d.%m.%Y", curTime);

    fprintf(logFile, "<html>\n"
                    "<head><title>%s</title><style>font {line-height: 0.8;} body {background-color: #404040;} head {background-color: #404040;}</style></head>\n"
                    "<body>\n"
                    "<h1><font color=\"99B333\">%s.</font></h1>\n", caption, date);

    TextOffset = ftell(logFile);

    fputs("</body>\n"
            "</html>\n", logFile);
    TextOffset -= ftell(logFile);
    free(date);
    return true;
}

/**
 * @brief      Закрывает файл логов.
*/
void LogDestructor()
{
    if (logFile)
        fclose(logFile);
}

/**
 * @brief                    Добавляет строку в файл логов.
 * @param file               Указатель на поток вывода.
 * @param message            Строка, которую необходимо добавить.
 * @param dublicateToConsole Если true, то дублирует сообщения в консоль. По умолчанию false
*/
void LogLine(const char* message, int logLevel, bool dublicateToConsole)
{
    assert(message);

    if (dublicateToConsole)
        puts(message);

    time_t rawTime = time(nullptr);
    tm* curTime = localtime(&rawTime);

    const char* dateFormat = "00:00:00 > ";
    size_t dateLength = strlen(dateFormat) + 1;
    char* date = (char*)calloc(dateLength, sizeof(char));

    if (date)
    {
        strftime(date, dateLength, "%H:%M:%S > ", curTime);
    
        fseek(logFile, TextOffset, SEEK_END);
        switch (logLevel)
        {
            case LOG_DEBUG:
                fprintf(logFile, "<pre><font color=\"5C65C0\">%s</font><font color=\"E0E0E0\">%s</font></pre>\n", date, message);
                break;
            case LOG_WARNING:
                fprintf(logFile, "<pre><font color=\"5C65C0\">%s</font><font color=\"E89827\">%s</font></pre>\n", date, message);
                break;
            case LOG_ERROR:
                fprintf(logFile, "<pre><font color=\"5C65C0\">%s</font><font color=\"E84127\">%s</font></pre>\n", date, message);
                break;
        }
        free(date);

        fputs("</body>\n"
                "</html>\n", logFile);
    }

    fflush(logFile);
}

