#include "region.h"
#include <QFile>
#include <QTime>
#include <QDebug>
#include "util.h"

/* This class contains information relating to Minecraft region files (*.mca).
 *
 * A region contains 32x32 (1024) chunks worth of data. It begins with a header that
 * contains offset and timestamp information. The rest of the file is chunk information
 * that is padded to 4096B. Note that uint32_t values must be corrected for endianness.
 *
 *   Header: [ uint32 location[1024] ][ uint32 timestamp[1024] ] (8192B total)
 *      The location field contains four bytes (big endian). After correcting the endian,
 *      the most significant three bytes indicate the offset of each chunk from the start
 *      of this file in 4kiB increments. Shift this value right by 8 to get keep that
 *      value, or shift left by 4 to get exact byte offsets. The least significant byte
 *      is the length of the chunk, also in 4kiB increments. Shift this left by 12 to get
 *      the exact data size. The timestamp field (also big endian) is the seconds since
 *      Januray 1, 1970. Chunks that have not been generated have a size and offset of 0,
 *      and do not take up space in the file beyond that recognition.
 *
 *   Chunk data: [ uint32 size ][ char compressionMethod ][ char data[size] ][ padding ]
 *      The chunk data begins with a big endian size (in bytes) of the data. This is
 *      followed by a char indicating the compression method (1 is GZip, 2 is ZLib).
 *      Recent Minecraft releases only use ZLib, making our lives easier, and the default
 *      ZLib uncompress() method works in decompressing the data. The rest of the chunk
 *      data is zero padded such that the next chunk will live at location xxxxx000 (hex).
 */

Region::Region(QString filename) : filename(filename), validChunks(0)
{
    QFile file(filename);

    if(file.open(QIODevice::ReadOnly))
    {
        int locations[1024];
        file.read((char*)locations, 4096); // Only read the offsets and sizes
        file.close();

        for(int i=0; i<1024; ++i)
        {
            chunks[i] = nullptr; // Initialize the chunk array to nulls
            if((_byteswap_ulong(locations[i]) & 0xFF) != 0) // Read the size fields
            {
                ++validChunks;
            }
        }
    }
}
Region::~Region()
{
    for(int i=0; i<1024; ++i)
    {
        if(chunks[i] != nullptr)
        {
            delete chunks[i];
        }
    }
}

int Region::size()
{
    return validChunks;
}
bool Region::load()
{
    qDebug() << "Loading region" << filename;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly) || file.size() < 0x3000)
    {
        return false;
    }

    int location, timestamp;
    Chunk* chunk;
    for(int i=0; i<1024; ++i)
    {
        file.seek(i*4);
        file.read((char*)&location, sizeof(int));
        file.seek(i*4 + 0x1000);
        file.read((char*)&timestamp, sizeof(int));

        chunk = new Chunk(swap<int>(location), swap<int>(timestamp));
        if(chunk->size > 0)
        {
            file.seek(chunk->offset);
            int size;
            file.read((char*)&size, sizeof(int));
            size = swap<int>(size);
            file.read(&(chunk->compressionType), sizeof(char));
            chunk->data = file.read(size);

            chunks[i] = chunk;
        }
        else
        {
            delete chunk;
        }
    }
    file.close();
    return true;
}
bool Region::save()
{
    //QFile file(filename);
    QFile file("C:/Users/Collin/AppData/Roaming/.minecraft/saves/ChunkLifterTest/region/r.-1.0.mca");
    file.remove();
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QByteArray locationHeader;
    QByteArray timestampHeader;
    unsigned int offset = 0x2000;

    qDebug() << "Generating file header";
    for(int i=0; i<1024; ++i)
    {
        if(chunks[i] != nullptr)
        {
            chunks[i]->offset = offset;
            offset += (chunks[i]->size << 12);

            locationHeader.append(swapArray<int>((chunks[i]->offset >> 4) | chunks[i]->size));
            timestampHeader.append(swapArray<int>(chunks[i]->time.toTime_t()));
        }
        else
        {
            locationHeader.append(4, 0x00);
            timestampHeader.append(4, 0x00);
        }
    }
    file.write(locationHeader);
    file.write(timestampHeader);

    qDebug() << "Writing chunk data";
    for(int i=0; i<1024; ++i)
    {
        if(chunks[i] != nullptr)
        {
            file.write(chunks[i]->data);
        }
    }
    qDebug() << "Region saved";

    file.close();
    return true;
}
