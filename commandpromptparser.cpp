
#include "commandpromptparser.h"

CommandPromptParser::CommandPromptParser(int argc, char* argv[])
    : m_params(getParamsFormArgs(argc, argv))
{
}

ParsedParams CommandPromptParser::getParamsFormArgs(int argc, char* argv[])
{
    ParsedParams params;

    DoPars(argc, argv, &params.firstVecSize, &params.lastVecSize, &params.incStep,
           &params.loopCount, &params.thCount, &params.thrCountInTest, &params.type);

    return params;
}

