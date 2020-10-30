#ifndef LAYOUT_H
#define LAYOUT_H

#include <QString>
#include <QVector>
#include <QVariant>
#include <QJsonValue>

namespace layout
{
    template<typename T_>
    inline QVariant getValue(T_ val_)
    {
        return QVariant::fromValue(val_);
    }


    template<typename T_>
    inline void setValue(T_ &dta_, const QVariant &var_)
    {
        dta_ = qvariant_cast<T_>(var_);
    }
};//namespace layout

template<typename DataType_>
class Layout;

template<typename DataType_>
class ILayoutItem
{
public:
    ILayoutItem(int index_, const QString &jsonName_, const QString &qmlName_)
        : m_index(index_),
          m_jsonName(jsonName_),
          m_qmlName(qmlName_)
    {
    }

    virtual int getModelIndex() const
    {
        return m_index;
    }

    virtual bool isJsonItem() const
    {
        return !m_jsonName.isEmpty();
    }

    virtual bool isQMLItem() const
    {
        return !m_qmlName.isEmpty();
    }

    virtual const QString &getJsonName() const
    {
        return m_jsonName;
    }

    virtual const QString &getQMLName() const
    {
        return m_qmlName;
    }

    virtual QVariant getValue(DataType_ *obj_) = 0;
    virtual void setValue(DataType_ *obj_, const QVariant& value_) = 0;

protected:
    int m_index = 0;
    QString m_jsonName;
    QString m_qmlName;
};


template<typename DataType_, typename ItemType_>
class LayoutField : public ILayoutItem<DataType_>
{
public:
    LayoutField(int index_,
                const QString &jsonName_,
                const QString &qmlName_,
                ItemType_ DataType_::*fieldPtr_
                )
        : ILayoutItem<DataType_>(index_, jsonName_, qmlName_),
          m_fieldPtr(fieldPtr_)
    {
    }

    virtual QVariant getValue(DataType_ *obj_)
    {
        return layout::getValue(obj_->*m_fieldPtr);
    }

    virtual void setValue(DataType_ *obj_, const QVariant& value_)
    {
        layout::setValue(obj_->*m_fieldPtr, value_);
    }

protected:
    ItemType_ DataType_::*m_fieldPtr = nullptr;

    friend class Layout<DataType_>;
};

template<typename DataType_, typename ModelType_>
class LayoutModelField : public ILayoutItem<DataType_>
{
public:
    using ModelTypePtr = ModelType_ *;
    using createModelFuncPtr = ModelTypePtr (DataType_::*)();

    LayoutModelField(int index_,
                     const QString &qmlName_,
                     ModelTypePtr DataType_::*modelPtr_,
                     createModelFuncPtr createFuncPtr_
                     )
        : ILayoutItem<DataType_>(index_, QString(), qmlName_),
          m_modelPtr(modelPtr_),
          m_createFuncPtr(createFuncPtr_)
    {
    }

    virtual QVariant getValue(DataType_ *obj_)
    {
        if(nullptr == obj_->*m_modelPtr)
        {
            obj_->*m_modelPtr = obj_->*m_createFuncPtr();
        }
        return layout::getValue(static_cast<QObject *>(obj_->*m_modelPtr));
    }

    virtual void setValue(DataType_ *obj_, const QVariant& value_)
    {
        Q_UNUSED(obj_);
        Q_UNUSED(value_);
        Q_ASSERT(false); // illegal use
    }

protected:
    ModelTypePtr DataType_::*m_modelPtr = nullptr;
    createModelFuncPtr m_createFuncPtr;
};

template<typename DataType_>
class Layout
{
public:
    Layout() = default;


private:
    QVector<ILayoutItem<DataType_> *> m_fields;
};


#endif // LAYOUT_H
