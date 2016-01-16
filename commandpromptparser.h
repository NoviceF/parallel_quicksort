#ifndef COMMANDPROMTPARSER_H
#define COMMANDPROMTPARSER_H

#include "progopimpl.h"

struct ParsedParams
{
    int firstVecSize;
    int lastVecSize;
    int incStep;
    int loopCount;
    int thCount;
    int thrCountInTest;
    FillType type;

};

class CommandPromptParser
{
public:
    CommandPromptParser(int argc, char* argv[]);
    const ParsedParams& getParams() const { return m_params; }

private:
    static ParsedParams getParamsFormArgs(int argc, char* argv[]);

private:
    const ParsedParams m_params;
};

#endif // COMMANDPROMTPARSER_H
