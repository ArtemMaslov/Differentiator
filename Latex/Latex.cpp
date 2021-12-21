#include <stdio.h>
#include <assert.h>
#include <Windows.h>
#include <stdarg.h>
#include <ctime>


#include "LatexTemplates.h"
#include "LatexReplacements.h"
#include "Latex.h"
#include "..\Logs\Logs.h"

#include "..\Math\MathExpression\MathDSL.h"


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

/**
 * @brief          Открывает файл в формате .tex и записывает в него общую информацию:
 *                 преамбулу, главную страницу, содержание, введение, теорию.
 * @param latex    Указатель на структуру Latex.
 * @param FileName Имя выходного файла без расширения.
 * @return         false, если не удалось открыть файл.
*/
bool OpenLatexArticle(Latex* latex, const char* fileName)
{
    assert(latex);
    assert(fileName);

    latex->file = fopen("article.tex", "w");
    latex->fileName = fileName;
    latex->problemCounter = 1;
    latex->repl.size = 15;
    latex->repl.nodes = (ReplNode*)calloc(latex->repl.size, sizeof(ReplNode));
    srand(time(nullptr));

    if (!latex->file)
    {
        LogLine("Ошибка открытия файла", LOG_ERROR, true);
        return false;
    }
    
    fputs(Preamble, latex->file);
    fputs(FirstPage, latex->file);
    //fputs(Contents, latex->file);
    fputs(Introduction, latex->file);
    fputs(Theory, latex->file);

    return true;
}

/**
 * @brief       Дописывает в конец файла список используемой литературы, закрывает его и компилирует документ.
 *              в формат pdf.
 * @param latex Указатель на структуру Latex.
*/
void CloseLatexArticle(Latex* latex)
{
    assert(latex);

    fputs(Literature, latex->file);

    char cmd[500] = "";
    
    fclose(latex->file);

    // Удаление старых файлов
    const char pathes[][4] =
    {
        "aux",
        "log",
        "toc",
        "pdf"
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

    // -1, потому что pdf удалять не надо!
    for (size_t st = 0; st < sizeof(pathes) / sizeof(pathes[0]) - 1; st++)
    {
        sprintf(cmd, "%s.%s", latex->fileName, pathes[st]);
        remove(cmd);
    }
}

/**
 * @brief         Записывает новую задачу в файл.
 * @param latex   Указатель на структуру Latex.
 * @param problem Дерево, содержащее производную, которую нужно взять.
*/
void LatexMathProblem(Latex* latex, MathTree* problem)
{
    if (!latex)
        return;

    assert(problem);

    latex->formulaCounter = 1;
    latex->formulaSkipCounter = 0;

    fprintf(latex->file, Problems, latex->problemCounter++);

    PrintMathNodeLatex(problem->root, latex->file);
    
    fputs(Solution, latex->file);
}

/**
 * @brief         Записывает ответ на задачу.
 * @param latex   Указатель на структуру Latex.
 * @param problem Условие задачи (функция, которую нужно продифференцировать).
 * @param answer  Ответ на задачу. (производная функции).
*/
void LatexMathProblemAnswer(Latex* latex, MathTree* problem, MathTree* answer)
{
    if (!latex)
        return;
    
    assert(problem);
    assert(answer);

    DoReplacement(answer->root, latex, true);
    fputs("\n\n\\textbf{Ответ:} \n", latex->file);
    
    fputs("\\begin{equation}\n", latex->file);
    PrintMathNodeLatex(answer->root, latex->file);
    fputs("\n\\end{equation}\n\n", latex->file);
    
    PrintReplacedNodes(latex);
}

/**
 * @brief              Записывает в файл случайную фразу из выбранных (sentenceType).
 * @param latex        Указатель на структуру Latex
 * @param sentenceType Код типов фраз, которые можно использовать (LatexTemplates.h)
*/
void LatexRandSentence(Latex* latex, int sentenceType)
{
    if (!latex)
        return;

    if (latex->formulaSkipCounter > 0)
    {
        latex->formulaSkipCounter--;
        if (latex->formulaSkipCounter > 0)
            return;
    }

    // Делаем рандомные пропуски в формулах
    if (rand() % 1000 <= ProbabilityMiss)
    {
        latex->formulaSkipCounter = FormulasSkipCount - 1;
        fputs("\n\nОпустим некоторые тривиальные рассуждения и перейдём к рассмотрению более значимых вопросов.\n\n", latex->file);
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

/**
 * @brief       Записывает в файл формулу в формате "(lval)` = rval".
 * @param latex Указатель на структуру Latex.
 * @param lval  Дифференцируемая функция.
 * @param rval  Приозводная функции.
*/
void LatexMathDiffFormula(Latex* latex, MathNode* lval, MathNode* rval)
{
    if (!latex || latex->formulaSkipCounter > 0)
        return;

    assert(lval);
    assert(rval);
    
    DoReplacement(lval, latex, true);
    fputs("\\begin{equation}\n(", latex->file);
    PrintMathNodeLatex(lval, latex->file);

    DoReplacement(rval, latex);
    fputs(")^\\prime = ", latex->file);
    PrintMathNodeLatex(rval, latex->file);
    
    fprintf(latex->file, 
            "\n\\label{eq:ref%zd}\n"
            "\\end{equation}", latex->formulaCounter++);
        
    PrintReplacedNodes(latex);
    fflush(latex->file);
}

/**
 * @brief       Записывает в файл формулу в формате "lval = rval".
 * @param latex Указатель на структуру Latex
 * @param lval  Функция слева от знака '='
 * @param rval  Функция справа от знака '='
*/
void LatexMathFormula(Latex* latex, MathNode* lval, MathNode* rval)
{
    if (!latex || latex->formulaSkipCounter > 0)
        return;

    assert(lval);
    //assert(rval);
    
    DoReplacement(lval, latex, true);
    fputs("\\begin{equation}\n", latex->file);
    PrintMathNodeLatex(lval, latex->file);

    if (rval != nullptr)
    {
        DoReplacement(rval, latex);
        fputs(" = ", latex->file);
        PrintMathNodeLatex(rval, latex->file);
    }
    fprintf(latex->file, 
            "\n\\label{eq:ref%zd}\n"
            "\\end{equation}", latex->formulaCounter++);
    
    PrintReplacedNodes(latex);
    fflush(latex->file);
}

/**
 * @brief       Записывает в файл строку.
 * @param latex Указатель на структуру Latex.
 * @param str   Строка.
*/
void LatexString(Latex* latex, const char* str)
{
    if (!latex || latex->formulaSkipCounter > 0)
        return;

    assert(str);

    fputs(str, latex->file);
    fputs("\n\n", latex->file);
    fflush(latex->file);
}

/**
 * @brief        Записывает в файл отформатированную строку.
 * @param latex  Указатель на структуру Latex.
 * @param format Формат строки (как в printf).
 * @param        Аргументы (как в printf).
*/
void LatexFormatedString(Latex* latex, const char* format, ...)
{
    if (!latex || latex->formulaSkipCounter > 0)
        return;

    assert(format);

    va_list args;
	va_start(args, format);
    vfprintf(latex->file, format, args);
    va_end(args);
    fflush(latex->file);
}

/**
 * @brief      Записывает поддерево в файл.
 * @param node Указатель на корень поддерева.
 * @param file Файл.
*/
void PrintMathNodeLatex(MathNode* node, FILE* file)
{
    assert(node);
    assert(file);

    switch (node->expression.type)
    {
        case ME_NUMBER:
        {
            fprintf(file, "%lg", node->expression.me_number);
            break;
        }
        case ME_CONSTANT:
        {
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
        }
        case ME_VARIABLE:
        {
            fputc(node->expression.me_variable, file);
            break;
        }
        case ME_FUNCTION:
        {
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

            bool parenthesesNeed = !(
                GET_FUNC(node) == ME_SQRT            ||
                GET_FUNC(node) == ME_CBRT            ||
                TYPE_EQUAL(RIGHT, ME_NUMBER)         || 
                TYPE_EQUAL(RIGHT, ME_CONSTANT)       ||
                TYPE_EQUAL(RIGHT, ME_VARIABLE)       ||
                TYPE_EQUAL(RIGHT, ME_OPERATOR)       &&
                GET_OPER(RIGHT) != ME_MULTIPLICATION &&
                GET_OPER(RIGHT) != ME_DIVISION);

            if (parenthesesNeed)
                fputc('(', file);
            PrintMathNodeLatex(node->nodeRight, file);
            if (parenthesesNeed)
                fputc(')', file);
            fputc('}', file);
            break;
    }
        case ME_OPERATOR:
        {
            switch (node->expression.me_operator)
            {
                case ME_ADDITION:
                case ME_SUBTRACTION:
                {
                    bool parenthesesNeed = !(
                        node->parent && 
                        ((GET_TYPE(node->parent, ME_OPERATOR) && GET_OPER(node->parent) == ME_DIVISION) ||
                         (GET_TYPE(node->parent, ME_FUNCTION) && 
                          (GET_FUNC(node->parent) == ME_SQRT || GET_FUNC(node->parent) == ME_CBRT))
                        ));

                    if (parenthesesNeed)
                        fputc('(', file);

                    PrintMathNodeLatex(node->nodeLeft, file);

                    if (GET_OPER(node) == ME_SUBTRACTION)
                        fputs(" - ", file);
                    else
                        fputs(" + ", file);

                    PrintMathNodeLatex(node->nodeRight, file);

                    if (parenthesesNeed)
                        fputc(')', file);
                    break;
                }
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
                {
                    bool parenthesesNeed = !(
                        TYPE_EQUAL(LEFT, ME_NUMBER)          || 
                        TYPE_EQUAL(LEFT, ME_CONSTANT)        ||
                        TYPE_EQUAL(LEFT, ME_VARIABLE)        ||
                        TYPE_EQUAL(LEFT, ME_OPERATOR)        &&
                        GET_OPER(RIGHT) != ME_MULTIPLICATION &&
                        GET_OPER(RIGHT) != ME_DIVISION);

                    if (parenthesesNeed)
                        fputc('(', file);
                    PrintMathNodeLatex(node->nodeLeft, file);
                    if (parenthesesNeed)
                        fputc(')', file);
                    fputs("^{", file);
                    PrintMathNodeLatex(node->nodeRight, file);
                    fputc('}', file);
                    break;
                }
            }
            break;
        }
    }
}

#include "..\Math\MathExpression\UndefMathDSL.h"
