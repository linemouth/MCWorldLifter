#ifndef CHUNKLIFTER_H
#define CHUNKLIFTER_H

#include <QThread>
#include "chunk.h"

class ChunkLifter : public QThread
{
    Q_OBJECT
public:
    ChunkLifter(Chunk* chunk);
    ~ChunkLifter();

    static void configure(int bottom, int amount);

signals:
    void complete();
    void skipped();

private:
    void run();

    Chunk* chunk_;
};

#endif // CHUNKLIFTER_H
