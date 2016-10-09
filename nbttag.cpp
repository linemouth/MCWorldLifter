#include "nbttag.h"
#include "util.h"

// Base tag class
void NbtTag::setHeader(char*& buffer, Type type, QString name)
{
    if(name.isNull())
    {
        if(type < INVALID)
        {
            type_ = type;
        }
        else
        {
            type_ = (Type)*buffer;
            ++buffer;
        }

        short length = swap<short>(*(short*)buffer);
        buffer += sizeof(short);
        name_ = QString(QByteArray(const_cast<char*>(buffer), length));
        buffer += length;
    }
    else
    {
        name_ = name;
        type_ = type;
    }
}
QByteArray NbtTag::getHeader()
{
    QByteArray out;
    out.append((char)type_);
    out.append(swapArray<short>(name_.size()));
    out.append(name_);
    return out;
}
QIcon NbtTag::getIcon()
{
    switch(type_)
    {
    case END:        return QIcon(":/icons/end");      break;
    case BYTE:       return QIcon(":/icons/byte");     break;
    case SHORT:      return QIcon(":/icons/short");    break;
    case INT:        return QIcon(":/icons/int");      break;
    case LONG:       return QIcon(":/icons/long");     break;
    case FLOAT:      return QIcon(":/icons/float");    break;
    case DOUBLE:     return QIcon(":/icons/double");   break;
    case BYTE_ARRAY: return QIcon(":/icons/binary");   break;
    case STRING:     return QIcon(":/icons/string");   break;
    case LIST:       return QIcon(":/icons/list");     break;
    case COMPOUND:   return QIcon(":/icons/compound"); break;
    case INT_ARRAY:  return QIcon(":/icons/array");    break;
    case INVALID:    return QIcon(":/icons/warning");  break;
    default:         return QIcon(":/icons/error");    break;
    }
}
QString NbtTag::getTypeName(Type type)
{
    if(type >= INVALID)
    {
        type = type_;
    }

    static const QStringList typeNames = QStringList() << "End" << "Byte" << "Short" << "Int" << "Long" << "Float" << "Double" << "Byte Array" << "String" << "List" << "Compound" << "Int Array";
    QString retVal;
    if(type < typeNames.size())
    {
        retVal = typeNames.at(type);
    }
    else
    {
        retVal = QString("Invalid type: %1").arg((int)type, 2, 16, QChar('0'));
    }
    return retVal;
}



// Byte
void NbtByte::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    data_ = *buffer;
    ++buffer;
}

// Short
void NbtShort::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    data_ = swap<short>(*(short*)buffer);
    buffer += sizeof(short);
}

// Int
void NbtInt::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    data_ = swap<int>(*(int*)buffer);
    buffer += sizeof(int);
}

// Long
void NbtLong::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    data_ = swap<long long>(*(long long*)buffer);
    buffer += sizeof(long long);
}

// Float
void NbtFloat::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    data_ = swap<float>(*(float*)buffer);
    buffer += sizeof(float);
}

// Double
void NbtDouble::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    data_ = swap<double>(*(double*)buffer);
    buffer += sizeof(double);
}

// Byte array methods
void NbtByteArray::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    int length = swap<int>(*(int*)buffer);
    buffer += sizeof(int);

    for(int i=0; i<length; ++i)
    {
        data_.append(*buffer);
        buffer += sizeof(char);
    }
}
QByteArray NbtByteArray::getData()
{
    QByteArray out = swapArray<int>(data_.size());
    out.reserve(data_.size() + 4);
    foreach(char i, data_)
    {
        out.append(i);
    }
    return out;
}

// String methods
void NbtString::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    int length = swap<short>(*(short*)buffer);
    buffer += sizeof(short);
    data_ = QString(QByteArray(buffer, length));
    buffer += length;
}
QByteArray NbtString::getData()
{
    QByteArray out = swapArray<short>(data_.size());
    out.append(data_.toLatin1());
    return out;
}

// List methods
NbtList::~NbtList()
{
    while(!data_.isEmpty())
    {
        NbtTag* tag = data_.takeLast();
        switch(tag->getType())
        {
        case BYTE:       delete (NbtByte*)tag;      break;
        case SHORT:      delete (NbtShort*)tag;     break;
        case INT:        delete (NbtInt*)tag;       break;
        case LONG:       delete (NbtLong*)tag;      break;
        case FLOAT:      delete (NbtFloat*)tag;     break;
        case DOUBLE:     delete (NbtDouble*)tag;    break;
        case BYTE_ARRAY: delete (NbtByteArray*)tag; break;
        case STRING:     delete (NbtString*)tag;    break;
        case LIST:       delete (NbtList*)tag;      break;
        case COMPOUND:   delete (NbtCompound*)tag;  break;
        case INT_ARRAY:  delete (NbtIntArray*)tag;  break;
        }
    }
}
void NbtList::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    subType_ = (Type)*buffer;
    ++buffer;
    int length = swap<int>(*(int*)buffer);
    buffer += sizeof(int);

    for(int i=0; i<length; ++i)
    {
        NbtTag* newTag;
        switch(subType_)
        {
        case BYTE:       newTag = new NbtByte(buffer, BYTE, "");            break;
        case SHORT:      newTag = new NbtShort(buffer, SHORT, "");          break;
        case INT:        newTag = new NbtInt(buffer, INT, "");              break;
        case LONG:       newTag = new NbtLong(buffer, LONG, "");            break;
        case FLOAT:      newTag = new NbtFloat(buffer, FLOAT, "");          break;
        case DOUBLE:     newTag = new NbtDouble(buffer, DOUBLE, "");        break;
        case BYTE_ARRAY: newTag = new NbtByteArray(buffer, BYTE_ARRAY, ""); break;
        case STRING:     newTag = new NbtString(buffer, STRING, "");        break;
        case LIST:       newTag = new NbtList(buffer, LIST, "");            break;
        case COMPOUND:   newTag = new NbtCompound(buffer, COMPOUND, "");    break;
        case INT_ARRAY:  newTag = new NbtIntArray(buffer, INT_ARRAY, "");   break;
        }
        data_.append(newTag);
    }
}
QByteArray NbtList::getData()
{
    QByteArray out;
    out.append((char)subType_);
    out.append(swapArray<int>(data_.size()));
    foreach(NbtTag* tag, data_)
    {
        out.append(tag->getData());
    }
    return out;
}
NbtTag* NbtList::getChild(int index)
{
    NbtTag* retVal = nullptr;
    if(index < data_.size())
    {
        retVal = data_.at(index);
    }
    return retVal;
}

// Compound methods
NbtCompound::~NbtCompound()
{
    while(!data_.isEmpty())
    {
        NbtTag* tag = data_.takeLast();
        switch(tag->getType())
        {
        case BYTE:       delete (NbtByte*)tag;      break;
        case SHORT:      delete (NbtShort*)tag;     break;
        case INT:        delete (NbtInt*)tag;       break;
        case LONG:       delete (NbtLong*)tag;      break;
        case FLOAT:      delete (NbtFloat*)tag;     break;
        case DOUBLE:     delete (NbtDouble*)tag;    break;
        case BYTE_ARRAY: delete (NbtByteArray*)tag; break;
        case STRING:     delete (NbtString*)tag;    break;
        case LIST:       delete (NbtList*)tag;      break;
        case COMPOUND:   delete (NbtCompound*)tag;  break;
        case INT_ARRAY:  delete (NbtIntArray*)tag;  break;
        }
    }
}
void NbtCompound::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    Type childType = (Type)*buffer;
    while(childType != END)
    {
        childType = (Type)*buffer;
        NbtTag* newTag;
        switch(childType)
        {
        case BYTE:       newTag = new NbtByte(buffer);      break;
        case SHORT:      newTag = new NbtShort(buffer);     break;
        case INT:        newTag = new NbtInt(buffer);       break;
        case LONG:       newTag = new NbtLong(buffer);      break;
        case FLOAT:      newTag = new NbtFloat(buffer);     break;
        case DOUBLE:     newTag = new NbtDouble(buffer);    break;
        case BYTE_ARRAY: newTag = new NbtByteArray(buffer); break;
        case STRING:     newTag = new NbtString(buffer);    break;
        case LIST:       newTag = new NbtList(buffer);      break;
        case COMPOUND:   newTag = new NbtCompound(buffer);  break;
        case INT_ARRAY:  newTag = new NbtIntArray(buffer);  break;
        default:         newTag = nullptr;                  break;
        }

        if(newTag)
        {
            data_.append(newTag);
        }
        else if(childType != END)
        {
            break;
        }
    }
    ++buffer; // Compound tags are terminated by an 'END' tag byte
}
QByteArray NbtCompound::getData()
{
    QByteArray out;
    foreach(NbtTag* tag, data_)
    {
        out.append(tag->getHeader());
        out.append(tag->getData());
    }
    out.append((char)END);
    return out;
}
NbtTag* NbtCompound::getChild(int index)
{
    NbtTag* retVal = nullptr;
    if(index < data_.size())
    {
        retVal = data_.at(index);
    }
    return retVal;
}
NbtTag* NbtCompound::getChild(QString name)
{
    NbtTag* retVal = nullptr;
    foreach(NbtTag* child, data_)
    {
        if(child->getName() == name)
        {
            retVal = child;
            break;
        }
    }
    return retVal;
}

// Int array methods
void NbtIntArray::setData(char*& buffer, Type type, QString name)
{
    setHeader(buffer, type, name);

    int length = swap<int>(*(int*)buffer);
    buffer += sizeof(int);

    for(int i=0; i<length; ++i)
    {
        data_.append(swap<int>(*(int*)buffer));
        buffer += sizeof(int);
    }
}
QByteArray NbtIntArray::getData()
{
    QByteArray out = swapArray<int>(data_.size());
    out.reserve(sizeof(int)*data_.size() + 4);
    foreach(int i, data_)
    {
        out.append(swapArray<int>(i));
    }
    return out;
}

// Tree methods
void NbtTree::setData(char* buffer)
{
    if(root != nullptr)
    {
        delete root;
    }

    switch((NbtTag::Type)*buffer)
    {
    case NbtTag::BYTE:       root = new NbtByte(buffer);      break;
    case NbtTag::SHORT:      root = new NbtShort(buffer);     break;
    case NbtTag::INT:        root = new NbtInt(buffer);       break;
    case NbtTag::LONG:       root = new NbtLong(buffer);      break;
    case NbtTag::FLOAT:      root = new NbtFloat(buffer);     break;
    case NbtTag::DOUBLE:     root = new NbtDouble(buffer);    break;
    case NbtTag::BYTE_ARRAY: root = new NbtByteArray(buffer); break;
    case NbtTag::STRING:     root = new NbtString(buffer);    break;
    case NbtTag::LIST:       root = new NbtList(buffer);      break;
    case NbtTag::COMPOUND:   root = new NbtCompound(buffer);  break;
    case NbtTag::INT_ARRAY:  root = new NbtIntArray(buffer);  break;
    default:                 root = nullptr;                  break;
    }
}
QByteArray NbtTree::getData()
{
    QByteArray out;
    if(root != nullptr)
    {
        out.append(root->getHeader());
        out.append(root->getData());
    }
    return out;
}
