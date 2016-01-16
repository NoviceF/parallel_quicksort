

#ifndef SORTIMPL_H
#define SORTIMPL_H
#include <vector>
#include <iostream>

using std::cout;
using std::endl;

template<typename T>
void exch(std::vector<T>& a, int x, int y)
{
    T temp = a[x];
    a[x] = a[y];
    a[y] = temp;
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
    //    cout << "after exch" << endl;
    //    cout << "start = " << *start << endl;
    //    cout << "end = " << *end << endl;
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

    //        cout << "before exit qsort" << endl;
    //        cout << "start = " << *start << endl;
    //        cout << "end = " << *end << endl;
    //    cout << "end addr = " << end << endl;
    //    cout << "i = " << *i << endl;
    //    quicksort(a, start, i-1);
    //    quicksort(a, i+1, end);
    return;
}
//
//
//#include <vector>
//#include <iostream>
//#include <ctime>
//#include <cstdlib>
//
//
//intvec merge_sort(intvec a);
//
//intvec merge(intvec a, intvec b);
//
//const int N = 1000000;
//
//int main() {
//
//    intvec a;
//    a.reserve(N);
//    srand(time(NULL));
//    for (int i = 0; i < N; ++i) {
//        a.push_back(rand() % 10000);
//    }
//
//    std::cout << "size of a = " << a.size() << std::endl;
//
//    a = merge_sort(a);
//
//
//    //    for(int i = 0;i < 1000/* static_cast<int>(a.size())*/;++i)
//    //    {
//    //        std::cout<<a[i]<<" ";
//    //    } 
//    return 0;
//}
typedef std::vector<int> intvec;

//intvec merge_sort(intvec a)
//{
//    if (a.size() <= 1)
//        return a;
//    intvec b, c;
//    b.assign(a.begin(), a.end()-(a.size() / 2));
//    c.assign(a.end()-(a.size() / 2), a.end());
//    return merge(merge_sort(b), merge_sort(c));
//}

//intvec merge(intvec a, intvec b)
//{
//    intvec c(a.size() + b.size());
//
//    int count1 = 0;
//    int count2 = 0;
//
//    for (int i = 0; i < static_cast<int> (c.size()); ++i)
//    {
//
//        if (count1 == static_cast<int> (a.size()))
//        {
//            c[i] = b[count2];
//            count2++;
//            continue;
//        }
//
//        if (count2 == static_cast<int> (b.size()))
//        {
//            c[i] = a[count1];
//            count1++;
//            continue;
//        }
//
//
//        if (a[count1] <= b[count2])
//        {
//            c[i] = a[count1];
//            count1++;
//        } else
//        {
//            c[i] = b[count2];
//            count2++;
//        }
//    }
//    return c;
//}


#endif  /* SORTIMPL_H */

