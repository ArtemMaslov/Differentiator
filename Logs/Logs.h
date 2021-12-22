#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>

extern int TextOffset;
extern size_t CreateGraphCallCount;

enum LogLevel
{
    LOG_DEBUG = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2
};

#define GRAPH_LOG_FOLDER "GraphicLog\\"
#define GRAPH_LOG_NAME "mathTreeGraph"

extern const char* GraphLogPath;

/**
 * @brief             Конструктор файла логов.
 * @param logFileName Имя выходного файла с логами.
 * @param caption     Заголовок файла логов.
*/
bool LogConstructor(const char* logFileName, const char* caption);

/**
 * @brief      Закрывает файл логов.
*/
void LogDestructor();

/**
 * @brief                    Добавляет строку в файл логов.
 * @param file               Указатель на поток вывода.
 * @param message            Строка, которую необходимо добавить.
 * @param dublicateToConsole Если true, то дублирует сообщения в консоль. По умолчанию false
*/
void LogLine(const char* message, const int logLevel, const bool dublicateToConsole = false);

void CreateHtmlGraphicLog(const char* imagesName);


#endif // !LOGS_H_
