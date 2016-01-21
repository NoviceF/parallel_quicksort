#include "sorttest.h"


void fillVectorFullRandom(std::vector<int>& vecToFill)
{
    srand(time(NULL));
    size_t vecSize = vecToFill.size();

    for (size_t i = 0; i < vecSize; ++i)
    {
        vecToFill.push_back(rand() % vecSize);
    }
}


void fillVectorHalfRandom(std::vector<int>& vecToFill)
{
    size_t vecSize = vecToFill.size();

    for (size_t i = 0; i < vecSize / 2; ++i)
    {
        vecToFill.push_back(i);
    }

    srand(time(NULL));

    for (size_t i = vecSize / 2 + 1; i < vecSize; ++i)
    {
        vecToFill.push_back(rand() % 100);
    }
}

////////////////////////////////////////////////////////////////////////////////

