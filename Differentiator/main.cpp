#include <stdio.h>
#include <Windows.h>


#include "..\Logs\Logs.h"
#include "..\Latex\Latex.h"
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
    fclose(file);

    Latex latex = {};
    if (!OpenLatexArticle(&latex, "article"))
    {
        puts("Ошибка открытия файла");
        return false;
    }
    
    LatexMathProblem(&latex, &diff.problem);

    if (!Differentiate(&diff, 'x', &latex))
    {
        puts("Ошибка дифференцирования");
        return false;
    }
    CreateHtmlGraphicLog(GRAPH_LOG_NAME);

    LatexMathProblemAnswer(&latex, &diff.problem, &diff.answer);
    CloseLatexArticle(&latex);
    //MathTreeOptimize(&diff.diffTree);
    
    //if (diff.diffTree.root)
    //    CreateTreeGraph("optimizedDiffTask.png", &diff.diffTree);
    
    DifferentiatorDestructor(&diff);

    return 0;
}