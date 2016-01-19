#ifndef SORT_H
#define SORT_H

#include <vector>

class ISort
{
public:
    virtual void makeSort() = 0;
    virtual ~ISort() = 0;
};

template <typename T>
class SortBase : public ISort
{
    SortBase(std::vector<T>& vecToSort);
    virtual ~SortBase();

    void makeSort()
    {
        doPreSortCheck();
        doSort();
        doPostSortCheck();
    }

private:
    virtual void doPreSortCheck();
    virtual void doSort() = 0;
    virtual void doPostSortCheck();

protected:
    std::vector<T>& m_vecToSort;
};

template <typename T>
class CSort : public SortBase<T>
{
public:
    CSort(std::vector<T>& vecToSort);
    void doSort() override;
};

template <typename T>
class STLSort : public SortBase<T>
{
public:
    STLSort(std::vector<T>& vecToSort);
    void doSort() override;
};

template <typename T>
class PosixParallelSort : public SortBase<T>
{
public:
    PosixParallelSort(std::vector<T>& vecToSort);
    void doSort() override;
};

template <typename T>
class Cpp11ParallelSort : public SortBase<T>
{
public:
    Cpp11ParallelSort(std::vector<T>& vecToSort);
    void doSort() override;
};


#endif // SORT_H
