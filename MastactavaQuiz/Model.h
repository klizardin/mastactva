#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include "Layout.h"


template<class DataType_>
class ListModelBaseOfData : public QAbstractListModel
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

protected:
    void loadListImpl()
    {
    }

    DataType_ *createItemImpl()
    {
    }

    void setItemImpl(int index_, DataType_ *item_)
    {
    }

    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

private:
    QVector<DataType_ *> m_data;
    QHash<int, QByteArray> m_roleNames;
};


#endif // MODEL_H
