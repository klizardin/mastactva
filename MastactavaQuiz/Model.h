#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QVector>
#include <QList>
#include <QHash>
#include <QVariant>
#include "Layout.h"


class IListModel
{
public:
    virtual ~IListModel() = default;
    virtual const QString &getLayoutName() const = 0;
    virtual QString getLayoutIdFiledName() const = 0;
    virtual int getCurrentIndex() const;
    virtual bool getCurrentIndexValues(QHash<QString, QVariant> &values_) const = 0;
    virtual QVariant getCurrentIndexIdFieldValue() const = 0;
    virtual QVariant getCurrentIndexFieldValue(const QString &jsonFieldName) const = 0;
};


template<class DataType_>
class ListModelBaseOfData : public QAbstractListModel, public IListModel
{
public:
    ListModelBaseOfData(QObject *parent_)
        :QAbstractListModel(parent_)
    {
        getDataLayout<DataType_>().initQMLModelRoleNames(m_roleNames);
    }

    virtual int rowCount(const QModelIndex &parent_) const override
    {
        Q_UNUSED(parent_);
        return m_data.size();
    }

    virtual QVariant data(const QModelIndex &index_, int role_) const override
    {
        const int row = index_.row();
        if(row < 0 || row >= m_data.size()) { return QVariant(); }
        const auto *item = m_data[row];
        return getDataLayout<DataType_>().getModelValue(item, role_);
    }

    virtual bool setData(const QModelIndex &index_, const QVariant &value_, int role_ = Qt::EditRole) override
    {
        const int row = index_.row();
        if(row < 0 || row >= m_data.size()) { return false; }
        auto *item = m_data[row];
        return getDataLayout<DataType_>().setModelValue(item, role_, value_);
    }

    virtual const QString &getLayoutName() const override
    {
        return getDataLayout<DataType_>().getLayoutJsonName();
    }

    virtual QString getLayoutIdFiledName() const override
    {
        return getDataLayout<DataType_>().getIdFieldJsonName();
    }

    virtual int getCurrentIndex() const override
    {
        return m_currentIndex;
    }

    virtual bool getCurrentIndexValues(QHash<QString, QVariant> &values_) const override
    {
        if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return false; }
        return getDataLayout<DataType_>().getJsonValues(m_data[m_currentIndex], values_);
    }

    virtual QVariant getCurrentIndexIdFieldValue() const override
    {
        if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return QVariant(); }
        return getDataLayout<DataType_>().getIdJsonValue(m_data[m_currentIndex]);
    }

    virtual QVariant getCurrentIndexFieldValue(const QString &jsonFieldName) const override
    {
        if(m_currentIndex < 0 || m_currentIndex >= m_data.size()) { return QVariant(); }
        return getDataLayout<DataType_>().getJsonValue(m_data[m_currentIndex], jsonFieldName);
    }

protected:
    void loadListImpl()
    {
    }

    DataType_ *createDataItemImpl()
    {
    }

    QVariant createItemImpl()
    {
        return QVariant::fromValue(createDataItemImpl());
    }

    bool setDataItemImpl(int index_, DataType_ *item_)
    {
    }

    bool setItemImpl(int index_, const QVariant &item_)
    {
        QObject *obj = qvariant_cast<QObject *>(item_);
        DataType_ *dataItem = qobject_cast<DataType_ *>(obj);
        if(nullptr == dataItem) { return false; }
        return setDataItemImpl(index_, dataItem);
    }

    void setLayoutJsonNameImpl(const QString &layoutJsonName_)
    {
        setDataLayout<DataType_>().setLayoutJsonName(layoutJsonName_);
    }

    void setLayoutIdImpl(const QString &fieldJsonName_)
    {
        setDataLayout<DataType_>().setIdField(fieldJsonName_);
    }

    void setLayoutRefImpl(const QString &fieldJsonName_, const QString &parentModel_, const QString &parentModelRefJsonName_)
    {
        setDataLayout<DataType_>().setRef(fieldJsonName_, parentModel_, parentModelRefJsonName_);
    }

    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

private:
    QVector<DataType_ *> m_data;
    QHash<int, QByteArray> m_roleNames;
    int m_currentIndex = -1;
};


#endif // MODEL_H
