#include <algorithm>
#include <iostream>
#include <iterator>

#include <program_options.hpp>

#include "commandpromptparser.h"

namespace po = boost::program_options;

std::vector<std::string> vecsSplit(std::string cDelimiter,
        const std::string& csStr)
{
    std::vector<std::string> vecRes;
    std::size_t uiPos = 0; //start of string
    std::size_t uiOldPos = 0; //start of string

    while (std::string::npos != (uiPos = csStr.find_first_of(cDelimiter, uiOldPos)))
    {
        vecRes.push_back(csStr.substr(uiOldPos, uiPos - uiOldPos));
        uiOldPos = uiPos + 1;
    }

    vecRes.push_back(csStr.substr(uiOldPos, -1));

    return vecRes;
}

CommandPromptParser::CommandPromptParser(int argc, char* argv[])
    : m_params(getParamsFormArgs(argc, argv))
{
}

ParsedParams CommandPromptParser::getParamsFormArgs(int argc, char* argv[])
{
    ParsedParams params;

    std::string str;
    int fillType = 0;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "produce help message")
                ("firstVecSize", po::value<size_t>(
                     &params.firstVecSize)->default_value(1000000),
                "size of vector to sort")
                ("lastVecSize", po::value<size_t>(
                     &params.lastVecSize)->default_value(1000000),
                "value of last of vectors, when using incrementing")
                ("incrementing_step", po::value<size_t>(
                     &params.incStep)->default_value(1),
                "incrementing_step")
                ("loop_count", po::value<size_t>(
                     &params.loopCount)->default_value(1),
                "count of loop")
                ("count_threads", po::value<size_t>(
                     &params.threadsCount)->default_value(2),
                "count of threads to use")
                ("test_threads_count",
                po::value<size_t>(&params.thrCountInTest)->default_value(0),
                "count of threads to use at the same time")
                ("fill_type", po::value<int>(&fillType)->default_value(0),
                "type of filling vector(0-full, 1-half)")
                ("param_string", po::value<std::string > (&str),
                "Using: \"vecFirst vecLast incr loop testTC fillType\"")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << "Usage: options_description [options]\n";
            std::cout << desc;
            std::exit(0);
        }

        if (!str.empty())
        {
            std::vector<std::string> vecStr = vecsSplit(" ", str);
            if (vecStr.size() != 6)
            {
                std::cout << "invalid arg count!" << std::endl;
                std::exit(1);
            }

            params.firstVecSize = ::lexical_cast<int, std::string> (vecStr[0]);
            params.lastVecSize = ::lexical_cast<int, std::string> (vecStr[1]);
            params.incStep = ::lexical_cast<int, std::string> (vecStr[2]);
            params.loopCount = ::lexical_cast<int, std::string> (vecStr[3]);
            params.thrCountInTest = ::lexical_cast<int, std::string> (vecStr[4]);
            params.type = static_cast<FillType>(fillType);

            if (!params.incStep)
                params.incStep = 1;
        }
        else
        {
            params.type = static_cast<FillType>(fillType);
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << std::endl << "Invalid using! " << ex.what();
        std::exit(1);
    }
    catch (...)
    {
        std::cout << std::endl << "Invalid using!";
        std::exit(1);
    }

    return params;
}

