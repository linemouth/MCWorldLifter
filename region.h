#ifndef REGION_H
#define REGION_H

#include "chunk.h"
#include <stdint.h>
#include <QString>
#include <QList>

class Region
{
public:
    uint32_t locations[1024];
    uint8_t sizes[1024];
    uint32_t timestamps[1024];

    Region(QString filename);
    ~Region();

    bool load();
    bool save();
    int size();
    Chunk* chunks[1024];

private:
    QString filename;
    uint32_t validChunks;
};

#endif // REGION_H
