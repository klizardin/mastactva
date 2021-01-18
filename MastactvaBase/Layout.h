#ifndef LAYOUT_H
#define LAYOUT_H


#include <type_traits>
#include <QString>
#include <QVector>
#include <QList>
#include <QHash>
#include <QVariant>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimeZone>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/utils.h"


template<typename DataType_>
class Layout;


namespace layout
{
    enum class JsonTypesEn : int
    {
        jt_null,
        jt_bool,
        jt_int,
        jt_double,
        jt_string,
        jt_datetime,
        jt_array,
        jt_object,
        jt_undefined
    };

    enum class SpecialFieldEn : int
    {
        none,
        appId,
        modelInfo,
        objectModelInfo
    };

    template<typename T_>
    inline QVariant getValue(const T_ &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue(val_);
    }

    inline QVariant getValue(const bool &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue((val_?1:0));
    }

    inline QVariant getValue(const QDateTime &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue(dateTimeToJsonString(val_));
    }

    template<typename T_>
    inline int compareValues(const T_ &val1_, const T_ &val2_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return val1_ < val2_ ? -1 : val2_ < val1_  ? 1 : 0;
    }

    template<typename T_>
    inline void setValue(const QVariant &var_, T_ &dta_)
    {
        dta_ = qvariant_cast<T_>(var_);
    }

    // TODO: add more types
    inline void setValue(const QJsonValue &var_, int &dta_)
    {
        if(var_.isNull() || !var_.isDouble()) { dta_ = 0; }
        else { dta_ = var_.toInt(dta_); }
    }

    inline void setValue(const QJsonValue &var_, bool &dta_)
    {
        if(var_.isNull() || !var_.isDouble()) { dta_ = false; }
        else { dta_ = var_.toInt(dta_) != 0; }
    }

    inline void setValue(const QJsonValue &var_, qreal &dta_)
    {
        if(var_.isNull() || !var_.isDouble()) { dta_ = 0.0; }
        else { dta_ = var_.toDouble(dta_); }
    }

    inline void setValue(const QJsonValue &var_, QString &dta_)
    {
        if(var_.isNull() || !var_.isString()) { dta_ = QString(); }
        else { dta_ = var_.toString(); }
    }

    inline void setValue(const QJsonValue &var_, QDateTime &dta_)
    {
        if(var_.isNull() || !var_.isString()) { dta_ = dateTimeFromJsonString(QString()); }
        else { dta_ = dateTimeFromJsonString(var_.toString()); }
    }

    template<typename T_>
    inline JsonTypesEn getJsonType(const T_ *)
    {
        return JsonTypesEn::jt_undefined;
    }

    inline JsonTypesEn getJsonType(const bool *)
    {
        return JsonTypesEn::jt_bool;
    }

    inline JsonTypesEn getJsonType(const int *)
    {
        return JsonTypesEn::jt_int;
    }

    inline JsonTypesEn getJsonType(const qreal *)
    {
        return JsonTypesEn::jt_double;
    }

    inline JsonTypesEn getJsonType(const QString *)
    {
        return JsonTypesEn::jt_string;
    }

    inline JsonTypesEn getJsonType(const QDateTime *)
    {
        return JsonTypesEn::jt_datetime;
    }

    namespace Private
    {
        template<typename DataType_>
        class ILayoutItem
        {
        public:
            ILayoutItem(int index_,
                        const QString &jsonName_,
                        const QString &qmlName_,
                        SpecialFieldEn type_ = SpecialFieldEn::none
                        )
                : m_index(index_),
                  m_jsonName(jsonName_),
                  m_qmlName(qmlName_),
                  m_type(type_)
            {
            }

            virtual ~ILayoutItem() = default;

            virtual int getModelIndex() const
            {
                return m_index;
            }

            virtual bool isJsonItem() const
            {
                return getType()==SpecialFieldEn::none && !m_jsonName.isEmpty();
            }

            virtual bool isQMLItem() const
            {
                return getType()==SpecialFieldEn::none && !m_qmlName.isEmpty();
            }

            virtual QString getJsonName() const
            {
                return getType()==SpecialFieldEn::none ? m_jsonName : QString();
            }

            virtual QString getQMLName() const
            {
                return getType()==SpecialFieldEn::none ? m_qmlName : QString();
            }

            virtual QVariant getValue(const DataType_ *obj_m, bool toQML_) const = 0;
            virtual void setValue(DataType_ *obj_, const QVariant& value_) const = 0;
            virtual void setValue(DataType_ *obj_, const QJsonValue& value_) const = 0;
            virtual int compareValues(const DataType_ *obj1_, const DataType_ *obj2_, bool toQML_) const = 0;
            virtual JsonTypesEn getJsonType() const = 0;

            bool isIdField() const
            {
                return m_idField;
            }

            void setFieldId(bool idField_)
            {
                m_idField = idField_;
            }

            SpecialFieldEn getType() const
            {
                return m_type;
            }

            bool autoCreated() const
            {
                return m_autoCreated;
            }

            void setAutoCreated(bool autoCreated_)
            {
                m_autoCreated = autoCreated_;
            }

        protected:
            int m_index = 0;
            QString m_jsonName;
            QString m_qmlName;
            bool m_idField = false;
            SpecialFieldEn m_type = SpecialFieldEn::none;
            bool m_autoCreated = false;
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

            virtual QVariant getValue(const DataType_ *obj_, bool toQML_) const override
            {
                if(nullptr == obj_) { return QVariant(); }
                return layout::getValue((obj_->*m_getFunc)(), toQML_);
            }

            virtual void setValue(DataType_ *obj_, const QVariant& value_) const override
            {
                if(nullptr == obj_) { return; }
                ItemType_ val;
                layout::setValue(value_, val);
                (obj_->*m_setFunc)(val);
            }

            virtual void setValue(DataType_ *obj_, const QJsonValue& value_) const override
            {
                if(nullptr == obj_) { return; }
                ItemType_ val;
                layout::setValue(value_, val);
                (obj_->*m_setFunc)(val);
            }

            virtual int compareValues(const DataType_ *obj1_, const DataType_ *obj2_, bool toQML_) const override
            {
                if(nullptr == obj1_ || nullptr == obj2_) { return 0; }
                return layout::compareValues((obj1_->*m_getFunc)(), (obj2_->*m_getFunc)(), toQML_);
            }

            virtual JsonTypesEn getJsonType() const override
            {
                const DataType_ *obj = nullptr;
                typename std::remove_reference<decltype((obj->*m_getFunc)())>::type *p = nullptr;
                return layout::getJsonType(p);
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
            using ModelTypeFieldPtr = ModelTypePtr DataType_::*;
            using createModelFuncPtr = ModelTypePtr (DataType_::*)();

            LayoutModelField(int index_,
                             const QString &qmlName_,
                             ModelTypeFieldPtr modelPtr_,
                             createModelFuncPtr createFuncPtr_
                             )
                : ILayoutItem<DataType_>(index_, QString(), qmlName_),
                  m_modelPtr(modelPtr_),
                  m_createFuncPtr(createFuncPtr_)
            {
                ILayoutItem<DataType_>::setAutoCreated(true);
            }

            virtual QVariant getValue(const DataType_ *obj_, bool toQML_) const override
            {
                if(nullptr == obj_) { return QVariant(); }
                if(!toQML_) { return QVariant(); }
                if(nullptr == obj_->*m_modelPtr)
                {
                    const_cast<DataType_ *>(obj_)->*m_modelPtr = (const_cast<DataType_ *>(obj_)->*m_createFuncPtr)();
                }
                return layout::getValue(static_cast<QObject *>(obj_->*m_modelPtr), toQML_);
            }

            virtual void setValue(DataType_ *obj_, const QVariant& value_) const override
            {
                if(nullptr == obj_) { return; }
                if(value_.isNull())
                {
                    delete (obj_->*m_modelPtr);
                    obj_->*m_modelPtr = nullptr;
                    return;
                }
                Q_ASSERT(false); // illegal use
            }

            virtual void setValue(DataType_ *obj_, const QJsonValue& value_) const override
            {
                Q_UNUSED(obj_);
                Q_UNUSED(value_);
                Q_ASSERT(false); // illegal use
            }

            virtual int compareValues(const DataType_ *obj1_, const DataType_ *obj2_, bool toQML_) const override
            {
                if(nullptr == obj1_ || nullptr == obj2_) { return 0; }
                if(nullptr == obj1_->*m_modelPtr)
                {
                    const_cast<DataType_ *>(obj1_)->*m_modelPtr = (const_cast<DataType_ *>(obj1_)->*m_createFuncPtr)();
                }
                if(nullptr == obj2_->*m_modelPtr)
                {
                    const_cast<DataType_ *>(obj2_)->*m_modelPtr = (const_cast<DataType_ *>(obj2_)->*m_createFuncPtr)();
                }
                return layout::compareValues(obj1_->*m_modelPtr, obj2_->*m_modelPtr, toQML_);
            }

            virtual JsonTypesEn getJsonType() const override
            {
                const DataType_ *obj = nullptr;
                typename std::remove_reference<decltype(obj->*m_modelPtr)>::type *p = nullptr;
                return layout::getJsonType(p);
            }

        protected:
            ModelTypeFieldPtr m_modelPtr = nullptr;
            createModelFuncPtr m_createFuncPtr;
        };

        template<typename DataType_, typename ItemType_>
        class LayoutSpecialField : public ILayoutItem<DataType_>
        {
        public:
            using itemMemberPtr = ItemType_ DataType_::*;

            LayoutSpecialField(int index_,
                             itemMemberPtr itemPtr,
                             SpecialFieldEn type_
                             )
                : ILayoutItem<DataType_>(index_, QString(), QString(), type_),
                  m_itemPtr(itemPtr)
            {
            }

            virtual QVariant getValue(const DataType_ *obj_, bool toQML_) const override
            {
                if(nullptr == obj_) { return QVariant(); }
                return layout::getValue(obj_->*m_itemPtr, toQML_);
            }

            virtual void setValue(DataType_ *obj_, const QVariant& value_) const override
            {
                if(nullptr == obj_) { return; }
                layout::setValue(value_, obj_->*m_itemPtr);
            }

            virtual void setValue(DataType_ *obj_, const QJsonValue& value_) const override
            {
                if(nullptr == obj_) { return; }
                layout::setValue(value_, obj_->*m_itemPtr);
            }

            virtual int compareValues(const DataType_ *obj1_, const DataType_ *obj2_, bool toQML_) const override
            {
                if(nullptr == obj1_ || nullptr == obj2_) { return 0; }
                return layout::compareValues(obj1_->*m_itemPtr, obj2_->*m_itemPtr, toQML_);
            }

            virtual JsonTypesEn getJsonType() const override
            {
                const DataType_ *obj = nullptr;
                typename std::remove_reference<decltype(obj->*m_itemPtr)>::type *p = nullptr;
                return layout::getJsonType(p);
            }

        protected:
            itemMemberPtr m_itemPtr = nullptr;
        };

    } // namesapce private
}//namespace layout


// TODO: review methods use, remove extra ones
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
        if(m_fields.back()->isQMLItem()) { m_lastQMLIndex++; }
    }

    template<typename ModelType_>
    void addModel(const QString &qmlName_,
                  typename layout::Private::LayoutModelField<DataType_,ModelType_>::ModelTypeFieldPtr varPtr_,
                  typename layout::Private::LayoutModelField<DataType_,ModelType_>::createModelFuncPtr createFunc_)
    {
        m_fields.push_back(new layout::Private::LayoutModelField<DataType_,ModelType_>(m_lastQMLIndex, qmlName_, varPtr_, createFunc_));
        if(m_fields.back()->isQMLItem()) { m_lastQMLIndex++; }
    }

    template<typename ItemType_>
    void addSpecial(layout::SpecialFieldEn type_,
                  typename layout::Private::LayoutSpecialField<DataType_,ItemType_>::itemMemberPtr varPtr_
                  )
    {
        m_fields.push_back(new layout::Private::LayoutSpecialField<DataType_,ItemType_>(m_lastQMLIndex, varPtr_, type_));
    }

    void initQMLModelRoleNames(QHash<int, QByteArray> &roleNames_) const
    {
        for(const layout::Private::ILayoutItem<DataType_> *layoutItem: m_fields)
        {
            if(nullptr == layoutItem || !layoutItem->isQMLItem()) { continue; }
            QString name = layoutItem->getQMLName();
            roleNames_[Qt::UserRole + layoutItem->getModelIndex()] = name.toLocal8Bit();
        }
    }

    QVariant getModelValue(const DataType_ *obj_, int role_) const
    {
        const layout::Private::ILayoutItem<DataType_> *layoutItem = findItemByModelRole(role_);
        if(nullptr == layoutItem) { return QVariant(); }
        return layoutItem->getValue(obj_, true);
    }

    bool setModelValue(DataType_ *obj_, int role_, const QVariant &val_) const
    {
        const layout::Private::ILayoutItem<DataType_> *layoutItem = findItemByModelRole(role_);
        if(nullptr == layoutItem) { return false; }
        layoutItem->setValue(obj_, val_);
        return true;
    }

    void createQMLValues(const DataType_ *obj_) const
    {
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(item->isQMLItem())
            {
                (void)item->getValue(obj_, true);
            }
        }
    }

    QVariant getJsonValue(const DataType_ *obj_, const QString &jsonFieldName_) const
    {
        const layout::Private::ILayoutItem<DataType_> *layoutItem = findItemByJsonName(jsonFieldName_);
        if(nullptr == layoutItem) { return QVariant(); }
        return layoutItem->getValue(obj_, false);
    }

    int compareJsonValues(const DataType_ *obj1_, const DataType_ *obj2_, const QString &jsonFieldName_) const
    {
        const layout::Private::ILayoutItem<DataType_> *layoutItem = findItemByJsonName(jsonFieldName_);
        if(nullptr == layoutItem) { return 0; }
        return layoutItem->compareValues(obj1_, obj2_, false);
    }

    QVariant getQMLValue(const DataType_ *obj_, const QString &qmlFieldName_) const
    {
        const layout::Private::ILayoutItem<DataType_> *layoutItem = findItemByQMLName(qmlFieldName_);
        if(nullptr == layoutItem) { return QVariant(); }
        return layoutItem->getValue(obj_, true);
    }

    bool getJsonValues(const DataType_ *obj_, QHash<QString, QVariant> &values_) const
    {
        bool ret = false;
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(!item->isJsonItem()) { continue; }
            values_[item->getJsonName()] = item->getValue(obj_, false);
            ret = true;
        }
        return ret;
    }

    bool setJsonValues(DataType_ *obj_, const QHash<QString, QVariant> &values_) const
    {
        bool ret = false;
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(!item->isJsonItem()) { continue; }
            if(!values_.contains(item->getJsonName())) { continue; }
            item->setValue(obj_, values_.value(item->getJsonName()));
            ret = true;
        }
        return ret;
    }

    bool setJsonValues(DataType_ *obj_, const QJsonDocument &jsonObj_, int index_ = 0) const
    {
        bool ret = false;
        if(jsonObj_.isArray())
        {
            QJsonValue jv = jsonObj_[index_];
            if(jv.isUndefined()) { return false; }
            ret |= setJsonValuesTempl(obj_, jv);
        }
        else if(jsonObj_.isObject())
        {
            ret |= setJsonValuesTempl(obj_, jsonObj_);
        }
        return ret;
    }

    bool setJsonValues(DataType_ *obj_, const QJsonObject &jsonObj_) const
    {
        return setJsonValuesTempl(obj_, jsonObj_);
    }

    bool setJsonValues(DataType_ *obj_, const QJsonValue &jsonObj_) const
    {
        return setJsonValuesTempl(obj_, jsonObj_);
    }

    const QString &getLayoutJsonName() const
    {
        return m_layoutJsonName;
    }

    void setLayoutJsonName(const QString &layoutJsonName_)
    {
        m_layoutJsonName = layoutJsonName_;
    }

    QString getIdFieldJsonName() const
    {
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(item->isIdField() && item->isJsonItem())
            {
                return item->getJsonName();
            }
        }
        return QString();
    }

    QVariant getIdJsonValue(const DataType_ *obj_) const
    {
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(item->isIdField() && item->isJsonItem())
            {
                return item->getValue(obj_, false);
            }
        }
        return QVariant();
    }

    void getJsonFieldsInfo(QList<QPair<QString, layout::JsonTypesEn>> &fieldsInfo_) const
    {
        fieldsInfo_.clear();
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(item->isJsonItem())
            {
                fieldsInfo_.push_back({item->getJsonName(), item->getJsonType()});
            }
        }
    }

    QVariant getSpecialFieldValue(layout::SpecialFieldEn type_, const DataType_ *obj_) const
    {
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(nullptr == item) { continue; }
            if(item->getType() == type_)
            {
                return item->getValue(obj_, true);
            }
        }
        return QVariant();
    }

    bool setSpecialFieldValue(layout::SpecialFieldEn type_, const QVariant &value_, DataType_ *obj_) const
    {
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(nullptr == item) { continue; }
            if(item->getType() == type_)
            {
                item->setValue(obj_, value_);
                return true;
            }
        }
        return false;
    }

    bool storeAfterSave() const
    {
        return m_storeAfterSave;
    }

    void setStoreAfterSave(bool storeAfterSave_)
    {
        m_storeAfterSave = storeAfterSave_;
    }

    void setIdField(const QString &fieldJsonName_)
    {
        layout::Private::ILayoutItem<DataType_> *layoutItem = findItemByJsonName(fieldJsonName_);
        Q_ASSERT(nullptr != layoutItem); // field not founded
        if(nullptr == layoutItem) { return; }
        layoutItem->setFieldId(true);
    }

    bool copyQMLFields(const DataType_ *from_, DataType_ *to_) const
    {
        bool ret = false;
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(nullptr == item) { continue; }
            if(item->autoCreated())
            {
                item->setValue(to_, QVariant::fromValue(static_cast<QObject *>(nullptr)));
                continue;
            }
            if(item->isQMLItem() || item->getType() != layout::SpecialFieldEn::none)
            {
                item->setValue(to_, item->getValue(from_, true));
                ret = true;
            }
        }
        return ret;
    }

protected:
    template<typename JsonType_>
    bool setJsonValuesTempl(DataType_ *obj_, const JsonType_ &jsonObj_) const
    {
        bool ret = false;
        for(const layout::Private::ILayoutItem<DataType_> *item : m_fields)
        {
            if(nullptr == item) { continue; }
            if(!item->isJsonItem()) { continue; }
            QJsonValue jv = jsonObj_[item->getJsonName()];
            if(jv.isUndefined()) { continue ; }
            item->setValue(obj_, jv);
            ret = true;
        }
        return ret;
    }

    const layout::Private::ILayoutItem<DataType_> *findItemByJsonName(const QString &fieldJsonName_) const
    {
        const auto fit = std::find_if(std::begin(m_fields), std::end(m_fields),
                     [&fieldJsonName_](layout::Private::ILayoutItem<DataType_> *item_)->bool
        {
            return nullptr != item_ && item_->getJsonName() == fieldJsonName_;
        });
        if(std::end(m_fields) == fit) { return nullptr; }
        return *fit;
    }

    layout::Private::ILayoutItem<DataType_> *findItemByJsonName(const QString &fieldJsonName_)
    {
        return const_cast<layout::Private::ILayoutItem<DataType_> *>
                (const_cast<const LayoutBase<DataType_> *>
                   (this)->findItemByJsonName(fieldJsonName_)
                );
    }

    const layout::Private::ILayoutItem<DataType_> *findItemByQMLName(const QString &fieldQMLName_) const
    {
        const auto fit = std::find_if(std::begin(m_fields), std::end(m_fields),
                     [&fieldQMLName_](layout::Private::ILayoutItem<DataType_> *item_)->bool
        {
            return nullptr != item_ && item_->getQMLName() == fieldQMLName_;
        });
        if(std::end(m_fields) == fit) { return nullptr; }
        return *fit;
    }

    layout::Private::ILayoutItem<DataType_> *findItemByQMLName(const QString &fieldQMLName_)
    {
        return const_cast<layout::Private::ILayoutItem<DataType_> *>
                (const_cast<const LayoutBase<DataType_> *>
                   (this)->findItemByQMLName(fieldQMLName_)
                );
    }

    const layout::Private::ILayoutItem<DataType_> *findItemByModelRole(int role_) const
    {
        const auto fit = std::find_if(std::begin(m_fields), std::end(m_fields),
                     [&role_](layout::Private::ILayoutItem<DataType_> *item_)->bool
        {
            return nullptr != item_ && item_->isQMLItem() && item_->getModelIndex() + Qt::UserRole == role_;
        });
        if(std::end(m_fields) == fit) { return nullptr; }
        return *fit;
    }

    layout::Private::ILayoutItem<DataType_> *findItemByModelRole(int role_)
    {
        return const_cast<layout::Private::ILayoutItem<DataType_> *>
                (const_cast<const LayoutBase<DataType_> *>
                   (this)->findItemByModelRole(role_)
                );
    }

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
    QString m_layoutJsonName;
    int m_lastQMLIndex = 0;
    QVector<layout::Private::ILayoutItem<DataType_> *> m_fields;
    bool m_storeAfterSave = true;
};


template<class DataType_> inline
const LayoutBase<DataType_> &getDataLayout()
{
    static typename DataType_::DefaultLayout staticItem;
    return staticItem;
}

template<class DataType_> inline
LayoutBase<DataType_> &setDataLayout()
{
    return const_cast<LayoutBase<DataType_> &>(getDataLayout<DataType_>());
}


#endif // LAYOUT_H
