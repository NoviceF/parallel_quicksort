#ifndef COMMANDPROMTPARSER_H
#define COMMANDPROMTPARSER_H

enum FillType //определяет способ заполнения вектора. fullSort - полное запол-
{               //нение случаными зачениями
    fullSort, halfSort //halfSort 1я половина отсортирована, вторая заполнена
};              //случайными значениями

struct ParsedParams
{
    size_t firstVecSize;
    size_t lastVecSize;
    size_t incStep;
    size_t loopCount;
    size_t threadsCount;
    size_t thrCountInTest;
    FillType type;

};

template<typename T2, typename T1>
inline T2 lexical_cast(const T1 &in)
{
    T2 out;
    std::stringstream ss;
    ss << in;
    ss >> out;
    return out;
}

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
