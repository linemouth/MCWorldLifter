#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include <stdint.h>
#include <QByteArray>

class Chunk
{
public:
    Chunk();
    ~Chunk();

    uint32_t bytes;
    uint8_t compressionType;
    QByteArray compressedData;
    QByteArray uncompressedData;
    //Block blocks[16][256][16];
};

#endif // CHUNK_H
