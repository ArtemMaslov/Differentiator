#include <stdio.h>


#include "..\Logs\Logs.h"
#include "Differentiator.h"


int main(int argc, char* argv[])
{
    if (!LogConstructor("Differentiator.log", "��� ��������� ��������������"))
    {
        puts("������ ��������� �������������� �������� ���������");
        return 1;
    }

    FILE* file = nullptr;
    const char* fileName = nullptr;
    if (argc > 1)
    {
        fileName = argv[1];
        file = fopen(fileName, "r");
    }
    else
    {
        fileName = "diffTask.txt";
        file = fopen(fileName, "r");
    }

    if (!file)
    {
        printf("������ �������� ����� \"%s\"", fileName);
        return 1;
    }

    Differentiator diff = {};
    DifferentiatorConstructor(&diff, file);


    DifferentiatorDestructor(&diff);

    return 0;
}