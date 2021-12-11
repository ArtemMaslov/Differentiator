#include <stdio.h>
#include <assert.h>


#include "Differentiator.h"
#include "Parser.h"
#include "..\Tree\Tree.h"
#include "..\Logs\Logs.h"


void DifferentiatorConstructor(Differentiator* diff, FILE* inputFile)
{
    TreeConstructor(&diff->tree);

    ReadFile(&diff->text, inputFile);


}

void DifferentiatorDestructor(Differentiator* diff)
{
    TreeDestructor(&diff->tree);

    TextDestructor(&diff->text);
}

