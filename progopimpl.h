#ifndef PROGOPIMPL_H
#define	PROGOPIMPL_H

enum FillType //определяет способ заполнения вектора. fullSort - полное запол-
{               //нение случаными зачениями
    fullSort, halfSort //halfSort 1я половина отсортирована, вторая заполнена
};              //случайными значениями

int DoPars(int argc, char* argv[], int* firstVecSize, int* lastVecSize,
           int *incStep, int* loopCount, int* thCount, int* thrCountInTest,
           FillType* fType);

#endif	/* PROGOPIMPL_H */


