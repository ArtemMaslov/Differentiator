#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>

extern int TextOffset;

enum LogLevel
{
    LOG_DEBUG = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2
};

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


#endif // !LOGS_H_
