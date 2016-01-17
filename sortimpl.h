#ifndef SORTIMPL_H
#define SORTIMPL_H
#include <vector>
#include <iostream>

using std::cout;
using std::endl;

static const int M = 15;

template<typename T>
void exch(std::vector<T>& a, int x, int y)
{
    if (x != y)
    {
        T temp = a[x];
        a[x] = a[y];
        a[y] = temp;
    }
}

template<typename T>
int partition(std::vector<T>& a, int* start, int* end)
{
    int i = *start - 1;
    int j = *end;
    T v = a[*end];

    for (;;)
    {
        while (a[++i] < v);
        while (v < a[--j]) if (j == *start) break;
        if (i >= j) break;
        exch(a, i, j);
    }
    exch(a, i, *end);

    return i;
}

template<typename T>
void quicksort(std::vector<T>& a, int* start, int* end, int* i)
{
    if (*end <= *start)
    {
        cout << "in qsort end <= start" << endl;
        cout << "start = " << *start << endl;
        cout << "i = " << *i << endl;
        cout << "end = " << *end << endl;

        return;
    }
    *i = partition(a, start, end);

    return;
}

#endif  /* SORTIMPL_H */

//template<typename T>
//void quicksort(std::vector<T>& a, int* start, int* end, int* i)
//{
//    if (*end <= *start)
//    {
//        cout << "in qsort end <= start" << endl;
//        cout << "start = " << *start << endl;
//        cout << "i = " << *i << endl;
//        cout << "end = " << *end << endl;
//
//        return;
//    }
//    int x = (*start + *end) / 2;
//    int y = *end / 2;
//    exch(a, x, y);
//    comexch(a, *start, *end - 1);
//    comexch(a, *start, *end);
//    comexch(a, *end - 1, *end);
//
//    *i = partition(a, start, end);
//
//    return;
//}
//template<typename T>
//void comexch(std::vector<T>& a, int start, int end)
//{
//    if (start < end) exch(a, start, end);
//}
//
//template<typename T>
//void insertion(std::vector<T>& a, int* start, int* end)
//{
//    int i;
//    for (i = *end; i > *start; --i) comexch(a, i - 1, i);
//    for (i = *start + 2; i <= *end; ++i)
//    {
//        int j = i;
//        T v = a[i];
//        while (v < a[j - 1])
//        {
//            a[j] = a[j - 1];
//            --j;
//        }
//        a[j] = v;
//    }
//    return;
//}
//
