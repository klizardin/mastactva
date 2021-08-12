#include "concents.h"

DataObject::DataObject(QObject *parent) : QObject(parent)
{
}

int DataObject::intValue() const
{
    return m_intValue;
}

void DataObject::setIntValue(int v_)
{
    m_intValue = v_;
}

QString DataObject::strValue() const
{
    return m_strValue;
}

void DataObject::setStrValue(const QString &v_)
{
    m_strValue = v_;
}

void testCreatePush()
{
    ModelConcept<DataType, DataObject> concept;
    for(int i = 0; i < 3; ++i)
    {
        auto obj = concept.create();
        Q_ASSERT(obj);
        concept.add(obj);
    }
}
