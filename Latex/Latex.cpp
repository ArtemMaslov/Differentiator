#include <stdio.h>
#include <assert.h>
#include <Windows.h>
#include <stdarg.h>
#include <ctime>


#include "LatexTemplates.h"
#include "Latex.h"


const char* MathSentencesCommon1[LATEX_COMMON1_SIZE] = 
{
    "Из курса математического анализа известно, что ",
    "Из курса элементарной алгебры известно, что ",
    "Вспомним программу 11 класса школьной алгебры: ",
    "Освежим формулы, забытые студентами за лето: ",
    "Вспомин формулы, которые студенты забыли после сдачи ЕГЭ: ",
    "Воспользуемся формулой, доказательство которой в данной методичке приведено не будет: ",
    "Читатель легко может показать, что ",
    "Не каждый первокурсник знает, но "
};

const char* MathSentencesCommon2[LATEX_COMMON2_SIZE] = 
{
    "Очевидно, что ",
    "Нетрудно показать, что ",
    "Не сложно показать, что ",
    "Всем известно, что ",
    "Легко видеть, что ",
};

const char* MathSentencesOperators[LATEX_OPERATOR_SIZE] = 
{
    "По теореме 4.2 из параграфа 1, IV глава 1 часть учебника А.Ю.Петровича \"Введение в математический анализ.\" (стр. 136) [1] ",
    "По теореме 3 из параграфа 9.5, I глава 1 том Кудрявцева Л.Д \"Курс математического анализа.\" (стр. 288) [2] ",
    "Пользуясь формулами 4-6 из параграфа 5.1, 5 глава, 1 том Никольского \"Курс математического анализа.\" (стр 130) [3], получаем, что ",
};

static void PrintMathNodeLatex(MathNode* node, FILE* file);


bool OpenLatexArticle(Latex* latex, const char* FileName)
{
    latex->file = fopen("article.tex", "w");
    latex->fileName = FileName;
    latex->problemCounter = 1;
    latex->variableReplacementSize = 15;
    latex->variableReplacement = (MathNode**)calloc(latex->variableReplacementSize, sizeof(MathNode));
    srand(time(nullptr));

    if (!latex->file)
    {
        printf("Ошибка открытия файла.");
        return false;
    }
    
    fputs(Preamble, latex->file);
    fputs(FirstPage, latex->file);
    //fputs(Contents, latex->file);
    //fputs(Introduction, latex->file);
    //fputs(Theory, latex->file);

    return true;
}

void CloseLatexArticle(Latex* latex)
{
    fputs(Literature, latex->file);

    char cmd[500] = "";
    
    fclose(latex->file);

    // Удаление старых файлов
    const char pathes[][4] =
    {
        "aux",
        "log",
        "pdf",
        "toc"
    };
    for (size_t st = 0; st < sizeof(pathes) / sizeof(pathes[0]); st++)
    {
        sprintf(cmd, "%s.%s", latex->fileName, pathes[st]);
        remove(cmd);
    }
    // Компиляция
    sprintf(cmd, "pdflatex %s.tex", latex->fileName);
    system(cmd);
    // Открытие
    sprintf(cmd, "%s.pdf", latex->fileName);
    system(cmd);
}

void LatexMathProblem(Latex* latex, MathTree* problem)
{
    if (!latex)
        return;

    latex->formulaCounter = 1;
    latex->formulaSkipCounter = 0;

    fputs(Problems, latex->file);

    PrintMathNodeLatex(problem->root, latex->file);
    
    fputs(Solution, latex->file);
}

void LatexMathProblemAnswer(Latex* latex, MathTree* problem, MathTree* answer)
{
    if (!latex)
        return;

    fputs("\n\n\\textbf{Ответ:} $", latex->file);
    PrintMathNodeLatex(answer->root, latex->file);

    fputs("$.", latex->file);
}

void LatexRandSentence(Latex* latex, int sentenceType)
{
    if (!latex)
        return;

    if (latex->formulaSkipCounter > 0)
    {
        latex->formulaSkipCounter--;
        return;
    }
    // Делаем рандомные пропуски в формулах
    if (rand() % 1000 <= ProbabilityMiss)
    {
        latex->formulaSkipCounter = FormulasSkipCount - 1;
        fputs("\nОпустим некоторые тривиальные рассуждения и перейдём к рассмотрению более значимых вопросов.\n", latex->file);
        return;
    }
    
    size_t arrCount = 0;
    int st = sentenceType;
    while (st > 0)
    {
        if (st % 2 == 1)
            arrCount++;
        st /= 2;
    }

    size_t index = 0;
    size_t power2 = 1;
    size_t arrIndex = rand() % arrCount;
    st = sentenceType;
    
    while (st >= power2)
    {
        if ((st & power2) > 0)
        {
            if (index == arrIndex)
            {
                switch (power2)
                {
                    case LATEX_COMMON1:
                        fputs(MathSentencesCommon1[rand() % LATEX_COMMON1_SIZE], latex->file);
                        return;
                    case LATEX_COMMON2:
                        fputs(MathSentencesCommon2[rand() % LATEX_COMMON2_SIZE], latex->file);
                        return;
                    case LATEX_OPERATOR:
                        fputs(MathSentencesOperators[rand() % LATEX_OPERATOR_SIZE], latex->file);
                        return;
                }
            }
            index++;
        }
        power2 *= 2;
    }
}

void LatexMathDiffFormula(Latex* latex, MathNode* lval, MathNode* rval)
{
    if (!latex || latex->formulaSkipCounter > 0)
        return;

    assert(lval);
    assert(rval);

    fputs("\\begin{equation}(", latex->file);

    PrintMathNodeLatex(lval, latex->file);

    fputs(")^\\prime = ", latex->file);
    PrintMathNodeLatex(rval, latex->file);
    
    fprintf(latex->file, 
            "\\label{eq:ref%zd}\n"
            "\\end{equation}\n\n", latex->formulaCounter);

    assert(lval);
}

void LatexMathFormula(Latex* latex, MathNode* lval, MathNode* rval)
{
    if (!latex || latex->formulaSkipCounter > 0)
        return;

    assert(lval);

    fputs("\\begin{equation}", latex->file);

    PrintMathNodeLatex(lval, latex->file);

    if (rval != nullptr)
    {
        fputs(" = ", latex->file);
        PrintMathNodeLatex(rval, latex->file);
    }
    fprintf(latex->file, 
            "\\label{eq:ref%zd}\n"
            "\\end{equation}\n\n", latex->formulaCounter);

    assert(lval);
}

void LatexString(Latex* latex, const char* str)
{
    if (!latex || latex->formulaSkipCounter > 0)
        return;

    fputs(str, latex->file);
    fputs("\n\n", latex->file);
}

void LatexFormatedString(Latex* latex, const char* format, ...)
{
    if (!latex || latex->formulaSkipCounter > 0)
        return;

    va_list args;
	va_start(args, format);
    vfprintf(latex->file, format, args);
    va_end(args);
}

static void PrintMathNodeLatex(MathNode* node, FILE* file)
{
    switch (node->expression.type)
    {
        case ME_NUMBER:
            fprintf(file, "%lg", node->expression.me_number);
            break;
        case ME_CONSTANT:
            switch (node->expression.me_constant)
            {
                case ME_PI:
                    fputs("\\pi", file);
                    break;
                case ME_EXP:
                    fputs("e", file);
                    break;
            }
            break;
        case ME_VARIABLE:
            fputc(node->expression.me_variable, file);
            break;
        case ME_FUNCTION:
            switch (node->expression.me_function)
            {
                case ME_SIN:
                    fputs("\\sin{", file);
                    break;
                case ME_COS:
                    fputs("\\cos{", file);
                    break;
                case ME_TG:
                    fputs("\\tan{", file);
                    break;
                case ME_CTG:
                    fputs("\\cot{", file);
                    break;
                case ME_SH:
                    fputs("\\sinh{", file);
                    break;
                case ME_CH:
                    fputs("\\cosh{", file);
                    break;
                case ME_LN:
                    fputs("\\ln{", file);
                    break;
                case ME_SQRT:
                    fputs("\\sqrt{", file);
                    break;
                case ME_CBRT:
                    fputs("\\cbrt{", file);
                    break;
                case ME_ARCSIN:
                    fputs("\\arcsin{", file);
                    break;
                case ME_ARCCOS:
                    fputs("\\arccos{", file);
                    break;
                case ME_ARCTG:
                    fputs("\\arctan{", file);
                    break;
                case ME_ARCCTG:
                    fputs("\\frac{\\pi}{2} - \\arctan{", file);
                    break;
            }
            PrintMathNodeLatex(node->nodeRight, file);
            fputc('}', file);
            break;
        case ME_OPERATOR:
            switch (node->expression.me_operator)
            {
                case ME_SUBTRACTION:
                    fputc('(', file);
                    PrintMathNodeLatex(node->nodeLeft, file);
                    fputs(" - ", file);
                    PrintMathNodeLatex(node->nodeRight, file);
                    fputc(')', file);
                    break;
                case ME_ADDITION:
                    fputc('(', file);
                    PrintMathNodeLatex(node->nodeLeft, file);
                    fputs(" + ", file);
                    PrintMathNodeLatex(node->nodeRight, file);
                    fputc(')', file);
                    break;
                case ME_MULTIPLICATION:
                    PrintMathNodeLatex(node->nodeLeft, file);
                    fputs(" \\cdot ", file);
                    PrintMathNodeLatex(node->nodeRight, file);
                    break;
                case ME_DIVISION:
                    fputs("\\frac{", file);
                    PrintMathNodeLatex(node->nodeLeft, file);
                    fputs("}{", file);
                    PrintMathNodeLatex(node->nodeRight, file);
                    fputc('}', file);
                    break;
                case ME_POWER:
                    fputc('(', file);
                    PrintMathNodeLatex(node->nodeLeft, file);
                    fputs(")^{", file);
                    PrintMathNodeLatex(node->nodeRight, file);
                    fputc('}', file);
                    break;
            }
            break;
    }
}

static void DoReplacement(MathNode* node, Latex* latex)
{
    if (node->nodeLeft)
        DoReplacement(node->nodeLeft, latex);
    if (node->nodeRight)
        DoReplacement(node->nodeRight, latex);


}