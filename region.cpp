#include "region.h"
#include <QFile>
#include <QTime>

Region::Region(QString filename) : filename(filename), validChunks(0)
{
    QFile file(filename);
    if(file.size() >= 0x3000) // Avoid opening empty files
    {
        if(file.open(QIODevice::ReadOnly))
        {
            file.read((char*)&locations, 4096);
            file.read((char*)&timestamps, 4096);
            file.close();

            for(int i=0; i<1024; ++i)
            {
                chunks[i] = nullptr;
                locations[i] = _byteswap_ulong(locations[i]);
                sizes[i] = locations[i] & 0xFF;
                locations[i] = (locations[i] << 4) & 0xFFFFF000;
                timestamps[i] = _byteswap_ulong(timestamps[i]);

                if(sizes[i] > 0)
                {
                    ++validChunks;
                }
            }
        }
    }
}
Region::~Region() {}

int Region::size()
{
    return validChunks;
}
bool Region::load()
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    if(file.size() < 1026*4096)
    {
        return false;
    }

    file.read((char*)&locations, 4096);
    file.read((char*)&timestamps, 4096);

    for(int i=0; i<1024; ++i)
    {
        qDebug() <<
        chunks[i] = nullptr;
        locations[i] = _byteswap_ulong(locations[i]);
        sizes[i] = locations[i] & 0xFF;
        locations[i] = (locations[i] << 4) & 0xFFFFF000;
        timestamps[i] = _byteswap_ulong(timestamps[i]);
        QDateTime timestamp;
        timestamp.setTime_t(timestamps[i]);

        if(sizes[i] > 0 && sizes[i] < 256)
        {
            chunks[i] = new Chunk();
            file.seek(locations[i]);
            file.read((char*)&chunks[i]->bytes, 4);
            file.read((char*)&chunks[i]->compressionType, 1);
            chunks[i]->bytes = _byteswap_ulong(chunks[i]->bytes);
            file.seek(locations[i] + 5);
            chunks[i]->compressedData.reserve(chunks[i]->bytes - 1);
            chunks[i]->compressedData = file.read(chunks[i]->bytes - 1);
        }
    }

    file.close();
    return true;
}
bool Region::save()
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.close();
    return true;
}
