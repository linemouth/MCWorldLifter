#include "chunklifter.h"
#include <QDebug>
#include <QFile>
#include "zlib.h"

static int bottom_ = 0;
static int amount_ = 0;
static const size_t bufferSize = 81920;

int decompress(QByteArray& in, QByteArray& out)
{
    int retVal = 0;

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    retVal = inflateInit(&stream);
    if (retVal != Z_OK)
    {
        qDebug() << "Zlib error during initialization:" << retVal;
        return retVal;
    }

    stream.avail_in = in.size();
    stream.next_in = (Bytef*)(in.data());
    char* buffer = new char[bufferSize];
    out.reserve(bufferSize);

    for(int i=0; retVal == 0; ++i)
    {
        stream.avail_out = bufferSize;
        stream.next_out = (Bytef*)(buffer);
        retVal = inflate(&stream, Z_NO_FLUSH);

        if(retVal >= 0)
        {
            QByteArray newData(buffer, bufferSize - stream.avail_out);
            out.append(newData);
        }
        else if(retVal < 0)
        {
            qDebug() << QString("Error (%1) on iteration %2").arg(retVal).arg(i);
        }
    }

    inflateEnd(&stream);
    if(retVal < 0)
    {
        return retVal;
    }

    return 0;
}
int compress(QByteArray& in, QByteArray& out)
{
    return 0;
}

ChunkLifter::ChunkLifter(Chunk* chunk) : chunk_(chunk) {}
ChunkLifter::~ChunkLifter() {}

void ChunkLifter::configure(int bottom, int amount)
{
    qDebug() << "ChunkLifter::configure()";
    bottom_ = bottom;
    amount_ = amount;
}
void ChunkLifter::run()
{
    qDebug() << "ChunkLifter::run()";
    if(chunk_ != nullptr)
    {
        int error = decompress(chunk_->compressedData, chunk_->uncompressedData);
        if(error >= 0)
        {
            if(chunk_->uncompressedData.size() > 0)
            {
                QFile file1(QString("C:/Users/Collin/Desktop/compressedData/file_%1.bin").arg(chunk_->bytes));
                file1.open(QIODevice::WriteOnly);
                file1.write(chunk_->compressedData);
                file1.close();

                QFile file2(QString("C:/Users/Collin/Desktop/uncompressedData/file_%1.bin").arg(chunk_->bytes));
                file2.open(QIODevice::WriteOnly);
                file2.write(chunk_->uncompressedData);
                file2.close();

                QByteArray recompressedData;
                recompressedData = qCompress(chunk_->uncompressedData);

                QFile file3(QString("C:/Users/Collin/Desktop/recompressedData/file_%1.bin").arg(chunk_->bytes));
                file3.open(QIODevice::WriteOnly);
                file3.write(recompressedData);
                file3.close();
            }
            else
            {
                qDebug() << "Decompress produced no data";
            }

            /*for(int x=0; x<16; ++x)
            {
                for(int z=0; z<16; ++z)
                {
                    int y = 255;

                    for(int y_s=(255-options_.amount); y_s >= options_.bottom; --y_s) // Copy the requested layer
                    {

                        Block block = chunk_->blocks[x][y_s][z]; // Get block to be raised
                        chunk_->blocks[x][y][z] = block;
                        --y;
                    }

                    for(; y != 0; --y) // Fill underneath with stone
                    {
                        chunk_->blocks[x][y][z] = Block(1);
                    }

                    // y is now 0- add a layer of bedrock
                    chunk_->blocks[x][0][z] = Block(7);
                }
            }*/
        }
        else
        {
            qDebug() << "Decompression error:" << error;
        }
    }

    emit complete();
}
