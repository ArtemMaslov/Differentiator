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

    size_t problemCount = 6;
    FILE* file = nullptr;
    char fileName[50] = "";
    
    Latex latex = {};
    if (!OpenLatexArticle(&latex, "article"))
    {
        puts("Ошибка открытия файла");
        return false;
    }

    for (size_t st = 2; st < problemCount; st++)
    {
        sprintf(fileName, "diffTask%d.txt", st);
        file = fopen(fileName, "r");

        if (!file)
        {
            printf("Ошибка открытия файла \"%s\"", fileName);
            continue;
        }

        Differentiator diff = {};
        DifferentiatorConstructor(&diff, file);
        fclose(file);
            
        LatexMathProblem(&latex, &diff.problem);

        if (!Differentiate(&diff, 'x', &latex))
        {
            puts("Ошибка дифференцирования");
            return false;
        }
        CreateHtmlGraphicLog(GRAPH_LOG_NAME);

        LatexMathProblemAnswer(&latex, &diff.problem, &diff.answer);

        DifferentiatorDestructor(&diff);
    }
    CloseLatexArticle(&latex);

    return 0;
}
