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
 * @brief             ����������� ����� �����.
 * @param logFileName ��� ��������� ����� � ������.
 * @param caption     ��������� ����� �����.
*/
bool LogConstructor(const char* logFileName, const char* caption);

/**
 * @brief      ��������� ���� �����.
*/
void LogDestructor();

/**
 * @brief                    ��������� ������ � ���� �����.
 * @param file               ��������� �� ����� ������.
 * @param message            ������, ������� ���������� ��������.
 * @param dublicateToConsole ���� true, �� ��������� ��������� � �������. �� ��������� false
*/
void LogLine(const char* message, int logLevel, bool dublicateToConsole = false);

void CreateHtmlGraphicLog(const char* imagesName);


#endif // !LOGS_H_
