#ifndef UTIL_H
#define UTIL_H

#include <QByteArray>

template <typename T>
T swap(T value)
{
    T retVal;
    int size = sizeof(T);
    for(int i=0; i<size; ++i)
    {
        ((char*)&retVal)[i] = ((char*)&value)[size - i - 1];
    }

    return retVal;
}
template <typename T>
QByteArray swapArray(T value)
{
    QByteArray retVal;
    int size = sizeof(T);
    for(int i=0; i<size; ++i)
    {
        retVal.append(((char*)&value)[size - i - 1]);
    }

    return retVal;
}

#endif // UTIL_H
