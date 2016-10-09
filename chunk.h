#ifndef CHUNK_H
#define CHUNK_H

#include <QByteArray>
#include <QThread>
#include <QObject>
#include "nbttag.h"
#include <QDateTime>

class Chunk : public QObject
{
    Q_OBJECT
public slots:
    void lift();

signals:
    void complete(int);

public:
    Chunk(unsigned int location, unsigned int timestamp);
    ~Chunk();

    unsigned int offset;
    unsigned int size;
    char compressionType;
    QDateTime time;
    QByteArray data;
    NbtTree tree;
};

#endif // CHUNK_H
