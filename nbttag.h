#ifndef NBTTAG_H
#define NBTTAG_H

#include "nbttag.h"
#include "util.h"
#include <QVariant>
#include <QString>
#include <QIcon>

// Forward Declarations
class NbtByte;
class NbtShort;
class NbtInt;
class NbtLong;
class NbtFloat;
class NbtDouble;
class NbtByteArray;
class NbtString;
class NbtList;
class NbtCompound;
class NbtIntArray;

class NbtTag
{
public:
    typedef enum : unsigned char
    {
        END = 0,
        BYTE = 1,
        SHORT = 2,
        INT = 3,
        LONG = 4,
        FLOAT = 5,
        DOUBLE = 6,
        BYTE_ARRAY = 7,
        STRING = 8,
        LIST = 9,
        COMPOUND = 10,
        INT_ARRAY = 11,
        INVALID = 0xFF
    } Type;

    NbtTag(Type type = INVALID, QString name = QString()) : type_(type), name_(name) {}
    virtual void setData(char*& buffer, Type type = INVALID, QString name = QString()) = 0;
    virtual QByteArray getData() = 0;
    QByteArray getHeader();
    virtual QVariant getText() = 0;
    QString getName() { return name_; }
    Type getType() { return type_; }
    QIcon getIcon();
    QString getTypeName(Type type = INVALID);
    virtual NbtTag* getChild(int) { return nullptr; }
    virtual NbtTag* getChild(QString) { return nullptr; }

    NbtByte*      asByte()      { return (NbtByte*)this; }
    NbtShort*     asShort()     { return (NbtShort*)this; }
    NbtInt*       asInt()       { return (NbtInt*)this; }
    NbtLong*      asLong()      { return (NbtLong*)this; }
    NbtFloat*     asFloat()     { return (NbtFloat*)this; }
    NbtDouble*    asDouble()    { return (NbtDouble*)this; }
    NbtByteArray* asByteArray() { return (NbtByteArray*)this; }
    NbtString*    asString()    { return (NbtString*)this; }
    NbtList*      asList()      { return (NbtList*)this; }
    NbtCompound*  asCompound()  { return (NbtCompound*)this; }
    NbtIntArray*  asIntArray()  { return (NbtIntArray*)this; }

protected:
    void setHeader(char*& buffer, Type type, QString name);
    Type type_;
    QString name_;
};



class NbtByte : public NbtTag
{
public:
    NbtByte(Type type = INVALID, QString name = QString(), char data = 0) : NbtTag(type, name), data_(data) {}
    NbtByte(char*& buffer, Type type = INVALID, QString name = QString()) { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    char* getValue() { return &data_; }
    void setValue(char value) { data_ = value; }
    QByteArray getData() { return QByteArray(1, data_); }
    QVariant getText() { return QVariant(data_); }

    char data_;
};

class NbtShort : public NbtTag
{
public:
    NbtShort(Type type = INVALID, QString name = QString(), short data = 0) : NbtTag(type, name), data_(data) {}
    NbtShort(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    short* getValue() { return &data_; }
    void setValue(short value) { data_ = value; }
    QByteArray getData() { return swapArray<short>(data_); }
    QVariant getText() { return QVariant(data_); }

    short data_;
};

class NbtInt : public NbtTag
{
public:
    NbtInt(Type type = INVALID, QString name = QString(), int data = 0) : NbtTag(type, name), data_(data) {}
    NbtInt(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    int* getValue() { return &data_; }
    void setValue(int value) { data_ = value; }
    QByteArray getData() { return swapArray<int>(data_); }
    QVariant getText() { return QVariant(data_); }

    int data_;
};

class NbtLong : public NbtTag
{
public:
    NbtLong(Type type = INVALID, QString name = QString(), long long data = 0) : NbtTag(type, name), data_(data) {}
    NbtLong(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    long long* getValue() { return &data_; }
    void setValue(long long value) { data_ = value; }
    QByteArray getData() { return swapArray<long long>(data_); }
    QVariant getText() { return QVariant(data_); }

    long long data_;
};

class NbtFloat : public NbtTag
{
public:
    NbtFloat(Type type = INVALID, QString name = QString(), float data = 0) : NbtTag(type, name), data_(data) {}
    NbtFloat(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    float* getValue() { return &data_; }
    void setValue(float value) { data_ = value; }
    QByteArray getData() { return swapArray<float>(data_); }
    QVariant getText() { return QVariant(data_); }

    float data_;
};

class NbtDouble : public NbtTag
{
public:
    NbtDouble(Type type = INVALID, QString name = QString(), double data = 0) : NbtTag(type, name), data_(data) {}
    NbtDouble(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    double* getValue() { return &data_; }
    void setValue(double value) { data_ = value; }
    QByteArray getData() { return swapArray<double>(data_); }
    QVariant getText() { return QVariant(data_); }

    double data_;
};

class NbtByteArray : public NbtTag
{
public:
    NbtByteArray(Type type = INVALID, QString name = QString(), QByteArray data = QByteArray()) : NbtTag(type, name), data_(data) {}
    NbtByteArray(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    QByteArray* getValue() { return &data_; }
    void setValue(QByteArray value) { data_ = value; }
    QByteArray getData();
    QVariant getText() { return QString("Elements: %1").arg(data_.size()); }

    QByteArray data_;
};

class NbtString : public NbtTag
{
public:
    NbtString(Type type = INVALID, QString name = QString(), QString data = QString()) : NbtTag(type, name), data_(data) {}
    NbtString(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    QString* getValue() { return &data_; }
    void setValue(QString value) { data_ = value; }
    QByteArray getData();
    QVariant getText() { return QString(data_); }

    QString data_;
};

class NbtList : public NbtTag
{
public:
    NbtList(Type type = INVALID, QString name = QString(), QList<NbtTag*> data = QList<NbtTag*>()) : NbtTag(type, name), data_(data) {}
    NbtList(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }
    ~NbtList();

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    QList<NbtTag*>* getValue() { return &data_; }
    void setValue(QList<NbtTag*> value) { data_ = value; }
    QByteArray getData();
    QVariant getText() { return QString("Elements: %1").arg(data_.size()); }
    NbtTag* getChild(int index);

    QList<NbtTag*> data_;
    Type subType_;
};

class NbtCompound : public NbtTag
{
public:
    NbtCompound(Type type = INVALID, QString name = QString(), QList<NbtTag*> data = QList<NbtTag*>()) : NbtTag(type, name), data_(data) {}
    NbtCompound(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }
    ~NbtCompound();

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    QList<NbtTag*>* getValue() { return &data_; }
    void setValue(QList<NbtTag*> value) { data_ = value; }
    QByteArray getData();
    QVariant getText() { return QString("Elements: %1").arg(data_.size()); }
    NbtTag* getChild(int index);
    NbtTag* getChild(QString name);

    QList<NbtTag*> data_;
};

class NbtIntArray : public NbtTag
{
public:
    NbtIntArray(Type type = INVALID, QString name = QString(), QList<int> data = QList<int>()) : NbtTag(type, name), data_(data) {}
    NbtIntArray(char*& buffer, Type type = INVALID, QString name = QString()) : NbtTag() { setData(buffer, type, name); }

    void setData(char*& buffer, Type type = INVALID, QString name = QString());
    QList<int>* getValue() { return &data_; }
    void setValue(QList<int> value) { data_ = value; }
    QByteArray getData();
    QVariant getText() { return QString("Elements: %1").arg(data_.size()); }

    QList<int> data_;
};

class NbtTree
{
public:
    NbtTree() : root(nullptr) {}
    NbtTree(char* buffer) : root(nullptr) { setData(buffer); }
    ~NbtTree() { delete root; }

    void setData(char* buffer);
    QByteArray getData();

    NbtTag* root;
};

#endif // NBTTAG_H
