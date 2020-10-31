#ifndef LAYOUT_H
#define LAYOUT_H

#include <QString>
#include <QVector>
#include <QList>
#include <QHash>
#include <QVariant>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>

template<typename DataType_>
class Layout;

inline
QDateTime dateTimeFromJsonString(const QString& dateTimeZ_)
{
    QString dateTimeZ(dateTimeZ_);
    QTextStream s(&dateTimeZ);
    int year = 0, month = 0, day = 0, hours = 0, minites = 0, seconds = 0;
    char tmp;
    s >> year >> tmp >> month >> tmp >> day >> tmp;
    s >> hours >> tmp >> minites >> tmp >> seconds;
    if(seconds > 100)
    {
        seconds /= 1000;
    }
    return QDateTime(QDate(year, month, day), QTime(hours, minites, seconds));
}

inline
QString dateTimeToJsonString(const QDateTime &dt_)
{
    return dt_.toString(Qt::DateFormat::ISODateWithMs);
}

namespace layout
{
    template<typename T_>
    inline QVariant getValue(const T_ &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue(val_);
    }

    inline QVariant getValue(const QDateTime &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue(dateTimeToJsonString(val_));
    }

    template<typename T_>
    inline void setValue(const QVariant &var_, T_ &dta_)
    {
        dta_ = qvariant_cast<T_>(var_);
    }

    inline void setValue(const QJsonValue &var_, int &dta_)
    {
        dta_ = var_.toInt(dta_);
    }

    inline void setValue(const QJsonValue &var_, qreal &dta_)
    {
        dta_ = var_.toDouble(dta_);
    }

    inline void setValue(const QJsonValue &var_, QDateTime &dta_)
    {
        dta_ = dateTimeFromJsonString(var_.toString());
    }

    inline void setValue(const QJsonValue &var_, QString &dta_)
    {
        dta_ = var_.toString();
    }

    namespace Private
    {
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

            virtual QVariant getValue(DataType_ *obj_m, bool toQML_) = 0;
            virtual void setValue(DataType_ *obj_, const QVariant& value_) const = 0;
            virtual void setValue(DataType_ *obj_, const QJsonValue& value_) const = 0;

        protected:
            int m_index = 0;
            QString m_jsonName;
            QString m_qmlName;
        };


        template<typename DataType_, typename ItemType_>
        class LayoutField : public ILayoutItem<DataType_>
        {
        public:
            using getFuncPtr = ItemType_ (DataType_::*)() const;
            using setFuncPtr = void (DataType_::*)(const ItemType_ &);

            LayoutField(int index_,
                        const QString &jsonName_,
                        const QString &qmlName_,
                        getFuncPtr getFunc_,
                        setFuncPtr setFunc_
                        )
                : ILayoutItem<DataType_>(index_, jsonName_, qmlName_),
                  m_getFunc(getFunc_),
                  m_setFunc(setFunc_)
            {
            }

            virtual QVariant getValue(DataType_ *obj_, bool toQML_) const override
            {
                return layout::getValue(obj_->*m_getFunc(), toQML_);
            }

            virtual void setValue(DataType_ *obj_, const QVariant& value_) const override
            {
                ItemType_ val;
                layout::setValue(value_, val);
                obj_->*setFunc(val);
            }

            virtual void setValue(DataType_ *obj_, const QJsonValue& value_) const override
            {
                ItemType_ val;
                layout::setValue(value_, val);
                obj_->*setFunc(val);
            }

        protected:
            getFuncPtr m_getFunc = nullptr;
            setFuncPtr m_setFunc = nullptr;

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

            virtual QVariant getValue(DataType_ *obj_, bool toQML_) const override
            {
                if(nullptr == obj_->*m_modelPtr)
                {
                    obj_->*m_modelPtr = obj_->*m_createFuncPtr();
                }
                return layout::getValue(static_cast<QObject *>(obj_->*m_modelPtr), toQML_);
            }

            virtual void setValue(DataType_ *obj_, const QVariant& value_) const override
            {
                Q_UNUSED(obj_);
                Q_UNUSED(value_);
                Q_ASSERT(false); // illegal use
            }

            virtual void setValue(DataType_ *obj_, const QJsonValue& value_) const override
            {
                Q_UNUSED(obj_);
                Q_UNUSED(value_);
                Q_ASSERT(false); // illegal use
            }

        protected:
            ModelTypePtr DataType_::*m_modelPtr = nullptr;
            createModelFuncPtr m_createFuncPtr;
        };
    }; // namesapce private
};//namespace layout


template<typename DataType_>
class LayoutBase
{
public:
    LayoutBase() = default;
    ~LayoutBase()
    {
        clearData();
    }

    template<typename ItemType_>
    void addField(const QString &jsonName_,
                  const QString &qmlName_,
                  typename layout::Private::LayoutField<DataType_,ItemType_>::getFuncPtr getFunc_,
                  typename layout::Private::LayoutField<DataType_,ItemType_>::setFuncPtr setFunc_)
    {
        m_fields.push_back(new layout::Private::LayoutField<DataType_,ItemType_>(m_lastQMLIndex, jsonName_, qmlName_, getFunc_, setFunc_));
        if(m_fields.back()->isQMLItem())
        {
            m_lastQMLIndex++;
        }
    }

    template<typename ModelType_>
    void addModel(const QString &qmlName_,
                  typename layout::Private::LayoutModelField<DataType_,ModelType_>::ModelTypePtr varPtr_,
                  typename layout::Private::LayoutModelField<DataType_,ModelType_>::createModelFuncPtr createFunc_)
    {
        m_fields.push_back(new layout::Private::LayoutModelField<DataType_,ModelType_>(m_lastQMLIndex, qmlName_, varPtr_, createFunc_));
        if(m_fields.back()->isQMLItem()) { m_lastQMLIndex++; }
    }

    void initQMLModelRoleNames(QHash<int, QByteArray> &roleNames_)
    {
        for(const layout::Private::ILayoutItem<DataType_> *layoutItem: m_fields)
        {
            if(nullptr == layoutItem || !layoutItem->isQMLItem()) { continue; }
            roleNames_[Qt::UserRole + layoutItem->getModelIndex()] = layoutItem->getQMLName();
        }
    }

    QVariant getModelValue(DataType_ *obj_, int role_) const
    {
        const auto fit = std::find_if(std::begin(m_fields), std::end(m_fields),
                                [role_](const layout::Private::ILayoutItem<DataType_> *item_)->bool
        {
            return nullptr != item_ && item_->isQMLItem() && item_->getModelIndex() + Qt::UserRole == role_;
        });
        if(std::end(m_fields) == fit) { return QVariant(); }
        const layout::Private::ILayoutItem<DataType_> *layoutItem = *fit;
        return layoutItem->getValue(obj_, true);
    }

    void setModelValue(DataType_ *obj_, int role_, const QVariant &val_) const
    {
        const auto fit = std::find_if(std::begin(m_fields), std::end(m_fields),
                                [role_](const layout::Private::ILayoutItem<DataType_> *item_)->bool
        {
            return nullptr != item_ && item_->isQMLItem() && item_->getModelIndex() + Qt::UserRole == role_;
        });
        if(std::end(m_fields) == fit) { return; }
        const layout::Private::ILayoutItem<DataType_> *layoutItem = *fit;
        layoutItem->setValue(obj_, val_);
    }

    void getJsonValues(DataType_ *obj_, QHash<QString, QVariant> &values_)
    {
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(!item->isJsonItem()) { continue; }
            values_[item->getJsonName()] = item->getValue(obj_, false);
        }
    }

    void setJsonValues(DataType_ *obj_, const QJsonDocument &jsonObj_)
    {
        setJsonValue(obj_, jsonObj_);
    }

    void setJsonValues(DataType_ *obj_, const QJsonObject &jsonObj_)
    {
        setJsonValue(obj_, jsonObj_);
    }

protected:
    template<typename JsonType_>
    void setJsonValues(DataType_ *obj_, const JsonType_ &jsonObj_)
    {
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(!item->isJsonItem()) { continue; }
            QJsonValue jv = jsonObj_[item->getJsonName()];
            if(jv.isUndefined()) { continue; }
            item->setValue(obj_, jv);
        }
    }

    void clearData()
    {
        for(auto *&p: m_fields)
        {
            delete p;
            p = nullptr;
        }
        m_fields.clear();
        m_lastQMLIndex = 0;
    }

private:
    int m_lastQMLIndex = 0;
    QVector<layout::Private::ILayoutItem<DataType_> *> m_fields;
};

template<class DataType_> inline
const typename DataType_::Layout &getDataLayout()
{
    static typename DataType_::Layout staticItem;
    return staticItem;
}

#endif // LAYOUT_H
