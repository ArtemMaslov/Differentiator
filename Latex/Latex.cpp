#include <stdio.h>
#include <assert.h>
#include <Windows.h>
#include <stdarg.h>
#include <ctime>


#include "LatexTemplates.h"
#include "Latex.h"


const char* MathSentencesCommon1[LATEX_COMMON1_SIZE] = 
{
    "�� ����� ��������������� ������� ��������, ��� ",
    "�� ����� ������������ ������� ��������, ��� ",
    "�������� ��������� 11 ������ �������� �������: ",
    "������� �������, ������� ���������� �� ����: ",
    "������� �������, ������� �������� ������ ����� ����� ���: ",
    "������������� ��������, �������������� ������� � ������ ��������� ��������� �� �����: ",
    "�������� ����� ����� ��������, ��� ",
    "�� ������ ������������ �����, �� "
};

const char* MathSentencesCommon2[LATEX_COMMON2_SIZE] = 
{
    "��������, ��� ",
    "�������� ��������, ��� ",
    "�� ������ ��������, ��� ",
    "���� ��������, ��� ",
    "����� ������, ��� ",
};

const char* MathSentencesOperators[LATEX_OPERATOR_SIZE] = 
{
    "�� ������� 4.2 �� ��������� 1, IV ����� 1 ����� �������� �.�.��������� \"�������� � �������������� ������.\" (���. 136) [1] ",
    "�� ������� 3 �� ��������� 9.5, I ����� 1 ��� ���������� �.� \"���� ��������������� �������.\" (���. 288) [2] ",
    "��������� ��������� 4-6 �� ��������� 5.1, 5 �����, 1 ��� ����������� \"���� ��������������� �������.\" (��� 130) [3], ��������, ��� ",
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
        printf("������ �������� �����.");
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

    // �������� ������ ������
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
    // ����������
    sprintf(cmd, "pdflatex %s.tex", latex->fileName);
    system(cmd);
    // ��������
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

    fputs("\n\n\\textbf{�����:} $", latex->file);
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
    // ������ ��������� �������� � ��������
    if (rand() % 1000 <= ProbabilityMiss)
    {
        latex->formulaSkipCounter = FormulasSkipCount - 1;
        fputs("\n������� ��������� ����������� ����������� � ������� � ������������ ����� �������� ��������.\n", latex->file);
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