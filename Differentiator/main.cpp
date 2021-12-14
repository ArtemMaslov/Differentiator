#include <stdio.h>
#include <Windows.h>


#include "..\Logs\Logs.h"
#include "Differentiator.h"

#include "..\Math\MathParser\MathParser.h"


int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (!LogConstructor("Differentiator.log", "Лог программы дифференциатор"))
    {
        puts("Работа программы дифференциатор аварийно завершена");
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
        fileName = "diffTask2.txt";
        file = fopen(fileName, "r");
    }

    if (!file)
    {
        printf("Ошибка открытия файла \"%s\"", fileName);
        return 1;
    }

    Differentiator diff = {};
    DifferentiatorConstructor(&diff, file);
    
    MathTree* diffTree = Differentiate(&diff, 'x');
    
    CreateTreeGraph("srcTask.png", &diff.tree);

    if (diffTree)
        CreateTreeGraph("diffTask.png", diffTree);
    
    FILE* outFile = fopen("outDiffTask.txt", "w");

    WriteTreeToFile(diffTree, outFile);

    fclose(outFile);

    DifferentiatorDestructor(&diff);
    fclose(file);

    return 0;
}