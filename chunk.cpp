#include "chunk.h"
#include "zlib.h"
#include "nbttag.h"
#include <QDebug>
#include <QFile>

/* This class contains information relating to Minecraft chunk data.
 *
 * Given that we ignored the data length and compression type in the region
 * file, we are left with a block of compressed data. To view it, we need
 * only decompress it using ZLib::uncompress(). The resulting data is in the
 * Named Binary Tag form, a data structure created by Markus Persson (Notch).
 * This structure contains a tree of tags with type, name, and length
 * identifiers.
 *
 *   Tag header: [ char type ][ short nameLength ][ char name[length] ]
 *      A tag begins with a single byte identifying the type of data it
 *      contains (values 0-11). Next is a two-byte (big endian) integer that
 *      denotes how many characters are present in the tag's name. This makes
 *      it possible to have spaces and special characters in a name, but
 *      Minecraft does not employ this property. Next is the name string
 *      which, because of the previous property, does not have a null
 *      terminatior. After that is data correlating the the data type stored
 *      in the tag. These types include the following type values:
 *
 *   0: End
 *      The null tag indicates the end of a compound type, essentially
 *      terminating its data. It does not have any data but its type value,
 *      and is therefore unnamed.
 *   1: Byte
 *      The bye tag is a primitive type that has a complete tag header
 *      followed by a a single-byte signed integer.
 *   2: Short
 *      Like the byte, this is a primitive type with a 2-byte signed integer.
 *   3: Int
 *      Contains a 4-byte signed integer.
 *   4: Long
 *      Contains a 8-byte signed integer.
 *   5: Float
 *      Contains a 4-byte floating point value.
 *   6: Double
 *      Contains a 8-byte floating point value.
 *   7: Byte Array
 *      The byte array tag is a more complex structure in that it contains an
 *      array of single-byte values. Its header is followed by a 4-byte
 *      big-endian array length, which is thus followed by the byte array.
 *   8: String
 *      The string tag is almost the same as the byte array tag. The only
 *      difference is that the length value is two bytes instead of four.
 *   9: List
 *      The list tag is a complex type that contains an array of any of the
 *      other types. After its header, there is a single byte indicating which
 *      type it contains. This is then followed by a four-byte length. The
 *      tags contained within it do not have headers, and are therefore unnamed.
 *      Because the types it contains can be of variable length, it cannot be
 *      indexed like an array.
 *   10: Compound
 *      The compound tag is like a list tag, except the tags it contains are
 *      fully-formed tags. Because of this, different tags can be contained in
 *      its structure. After its header comes its first child tag. These child
 *      tags continue indefinitely. The end of the child tag array is denoted by
 *      a single-byte end tag.
 *   11: Int Array
 *      The int array tag is like a byte array tag except that its array is
 *      made from four-byte, big-endian values.
 */

Chunk::Chunk(unsigned int location, unsigned int timestamp)
{
    offset = (location << 4) & 0xFFFFF000;
    size = location & 0xFF;
    time.setTime_t(timestamp);
}
Chunk::~Chunk() {}

static const int bufferSize = 0x800000;

NbtCompound* makeSection(char y)
{
    // Creates a new chunk section for filling the chunk with stone
    NbtCompound* section = new NbtCompound;
    section->data_.append(new NbtByteArray(NbtTag::BYTE_ARRAY, "Blocks", QByteArray(4096, 1)));
    section->data_.append(new NbtByteArray(NbtTag::BYTE_ARRAY, "SkyLight", QByteArray(2048, 0)));
    section->data_.append(new NbtByte(NbtTag::BYTE, "Y", y));
    section->data_.append(new NbtByteArray(NbtTag::BYTE_ARRAY, "BlockLight", QByteArray(2048, 0)));
    section->data_.append(new NbtByteArray(NbtTag::BYTE_ARRAY, "Data", QByteArray(2048, 0)));

    QByteArray* blocks = &((NbtByteArray*)(section->getChild("Blocks")))->data_;
    if(y == 0) // This is the bottom section
    {
        for(int i=0; i<256; ++i)
        {
            (*blocks)[i] = 7; // Set it to bedrock
        }
    }

    return section; // Return the section and hand memory management to the caller
}
void Chunk::lift()
{
    // Uncompress the raw chunk data
    int bufferSize = 0x800000;
    char* buffer = new char[bufferSize];
    int status = uncompress((Bytef*)buffer, (uLongf*)&bufferSize, (Bytef*)data.data(), data.size());

    if(status >= 0) // If uncompression succeeded, continue
    {
        tree.setData(buffer); // This parses the uncompressed data into the tree structure
        QList<NbtTag*>* sectionList = &tree.root->getChild("Level")->getChild("Sections")->asList()->data_;
        QList<NbtTag*>* workingList = new QList<NbtTag*>; // This will be our local copy
        /*for(int i=0; i<9; ++i) // Make 8 new sections of stone at the bottom half of the map
        {
            workingList->append(makeSection(i));
        }*/
        /*while(!sectionList->isEmpty()) // Lift and move relevant sections to the working list and delete the others
        {
            NbtCompound* tag = sectionList->takeFirst()->asCompound();
            if(tag->getChild("Y")->asByte()->data_ > 7 || tag->getChild("Y")->asByte()->data_ == 0) // It's beyond the world elevation limit
            {
                delete tag; // Correctly deletes tag as a Compound type
            }
            else
            {
                tag->getChild("Y")->asByte()->data_ += 8; // Lift it
                workingList->append(tag); // Append it to the list
            }
        }
        tree.root->getChild("Level")->getChild("Sections")->asList()->data_ = *workingList; // Replace the list with our working copy, hand memory management responsibility to the tree
*/
        // Update the heightmap
        /*foreach(int i, tree.root->getChild("Level")->getChild("HeightMap")->asIntArray()->data_)
        {
            i = qMin(i, 255);
        }*/

        // Repack the tree data into a bytestream and compress it
        QByteArray array = tree.getData();

        bufferSize = 0x800000;
        status = compress((Bytef*)buffer, (uLongf*)&bufferSize, (Bytef*)array.data(), array.size());

        if(status < 0)
        {
            qDebug() << "Compression failed:" << status;
        }
        else
        {
            data = QByteArray(buffer, bufferSize);
        }
    }

    data.prepend(0x02); // Compression method
    data.prepend(swapArray<int>(data.size() - 1)); // Size
    data.append(((data.size() + 0xFFF) & 0xFFFFF000) - data.size(), 0x00); // Padding
    // Data should now be a multiple of 4kiB long

    emit complete(status); // This returns any failures caused by compression
}
