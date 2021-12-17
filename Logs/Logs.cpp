#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <assert.h>
#include <direct.h>


#include "Logs.h"


int TextOffset = 0;
size_t CreateGraphCallCount = 0;

static FILE* logFile = nullptr;
const char* GraphLogPath = GRAPH_LOG_FOLDER GRAPH_LOG_NAME;


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

    _mkdir(GRAPH_LOG_FOLDER);

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

void CreateHtmlGraphicLog(const char* imagesName)
{
    assert(imagesName);

    FILE* file = fopen(GRAPH_LOG_FOLDER "GraphicLog.html", "w");
    if (!file)
    {
        puts("Ошибка открытия файла.");
        return;
    }

    const char* pattern = "<html>\n"
                          "<head><title>Лог программы \"Дифференциатор\".</title><style>font{line - height: 0.8; } body{background - color: #404040; } head{background - color: #404040; }</style></head>\n"
                          "<body>\n"
                          "<h1><font color = \"99B333\">Лог программы \"Дифференциатор\".</font></h1>\n";

    const char* ending  = "</body>\n</html>";

    fputs(pattern, file);

    for (int st = 1; st <= CreateGraphCallCount; st++)
        fprintf(file, "<img src = \"%s%d.png\">\n", imagesName, st);

    fputs(ending, file);

    fclose(file);
}