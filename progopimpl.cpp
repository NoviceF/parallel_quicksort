#include <algorithm>
#include <iostream>
#include <iterator>

#include <boost/program_options.hpp>

#include "progopimpl.h"

namespace po = boost::program_options;

std::vector<std::string> vecsSplit(std::string cDelimiter,
        const std::string & csStr)
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

template<typename T2, typename T1>
inline T2 lexical_cast(const T1 &in)
{
    T2 out;
    std::stringstream ss;
    ss << in;
    ss >> out;
    return out;
}

int DoPars(int ac, char* argv[], int* firstVecSize, int* lastVecSize, int *incStep,
        int* loopCount, int* thCount, int* thrCountInTest, FillType* fType)
{
    using namespace std;

    string str;
    int fillType = 0;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "produce help message")
                ("firstVecSize", po::value<int>(firstVecSize)->default_value(1000000),
                "size of vector to sort")
                ("lastVecSize", po::value<int>(lastVecSize)->default_value(1000000),
                "value of last of vectors, when using incrementing")
                ("incrementing_step", po::value<int>(incStep)->default_value(1),
                "incrementing_step")
                ("loop_count", po::value<int>(loopCount)->default_value(1),
                "count of loop")
                ("count_threads", po::value<int>(thCount)->default_value(2),
                "count of threads to use")
                ("test_threads_count",
                po::value<int>(thrCountInTest)->default_value(0),
                "count of threads to use at the same time")
                ("fill_type", po::value<int>(&fillType)->default_value(0),
                "type of filling vector(0-full, 1-half)")
                ("param_string", po::value<std::string > (&str),
                "Using: \"vecFirst vecLast incr loop testTC\"")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, argv, desc), vm);
        po::notify(vm);

        //        *fType = static_cast<FillType> (typeOfFilling);

        if (vm.count("help"))
        {
            cout << "Usage: options_description [options]\n";
            cout << desc;
            return -1;
        }
        //
        //    if (vm.count("include-path"))
        //    {
        //        cout << "Include paths are: "
        //                << vm["include-path"].as< vector<string> >() << "\n";
        //    }
        //
        //    if (vm.count("input-file"))
        //    {
        //        cout << "Input files are: "
        //                << vm["input-file"].as< vector<string> >() << "\n";
        //    }
        //
        //    if (vm.count("verbose"))
        //    {
        //        cout << "Verbosity enabled.  Level is " << vm["verbose"].as<int>()
        //                << "\n";
        //    }
        //
        //    cout << "size vector to sort is " << vecS << "\n";
        //    cout << "type of filling vector is " << typeOfFilling << "\n";
        //    cout << "count of threads to use is " << *thCount << "\n";

        //    int vb = 0;
        //    int ve = 0;
        //    int is = 0;
        //    int lc = 0;
        if (!str.empty())
        {
            vector<string> vecStr = vecsSplit(" ", str);
            if (vecStr.size() != 6)
            {
                cout << "invalid arg count!" << endl;
                return -1;
            }
            *firstVecSize = ::lexical_cast<int, std::string > (vecStr[0]);
            *lastVecSize = ::lexical_cast<int, std::string > (vecStr[1]);
            *incStep = ::lexical_cast<int, std::string > (vecStr[2]);
            *loopCount = ::lexical_cast<int, std::string > (vecStr[3]);
            *thrCountInTest = ::lexical_cast<int, std::string > (vecStr[4]);
            *fType = static_cast<FillType>(fillType);

        }
    } catch (...)
    {
        cout << endl << "Invalid using!";
        return -1;
    }
    
    return 0;
}


