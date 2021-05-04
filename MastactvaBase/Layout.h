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
#include "../MastactvaBase/layout_enums.h"
#include "../MastactvaBase/layout_variant.h"
#include "../MastactvaBase/layout_type.h"
#include "../MastactvaBase/layout_json.h"
#include "../MastactvaBase/utils.h"


template<typename DataType_>
class Layout;


namespace layout
{
    namespace Private
    {
        template<typename DataType_>
        class ILayoutItem
        {
        public:
            virtual ~ILayoutItem() = default;
            virtual int getModelIndex() const = 0;
            virtual bool isJsonItem() const = 0;
            virtual bool isQMLItem() const = 0;
            virtual QString getJsonName() const = 0;
            virtual QString getQMLName() const = 0;
            virtual QVariant getValue(const DataType_ *obj_m, bool toQML_) const = 0;
            virtual void setValue(DataType_ *obj_, const QVariant& value_) const = 0;
            virtual void setValue(DataType_ *obj_, const QJsonValue& value_) const = 0;
            virtual int compareValues(const DataType_ *obj1_, const DataType_ *obj2_, bool toQML_) const = 0;
            virtual JsonTypesEn getJsonType() const = 0;
        };

        template<typename DataType_>
        class ILayoutItemBase : public ILayoutItem<DataType_>
        {
        public:
            ILayoutItemBase(int index_,
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
        class LayoutField : public ILayoutItemBase<DataType_>
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
                : ILayoutItemBase<DataType_>(index_, jsonName_, qmlName_),
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
        class LayoutModelField : public ILayoutItemBase<DataType_>
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
                : ILayoutItemBase<DataType_>(index_, QString(), qmlName_),
                  m_modelPtr(modelPtr_),
                  m_createFuncPtr(createFuncPtr_)
            {
                ILayoutItemBase<DataType_>::setAutoCreated(true);
            }

            virtual QVariant getValue(const DataType_ *obj_, bool toQML_) const override
            {
                if(nullptr == obj_) { return QVariant(); }
                if(!toQML_) { return QVariant(); }
                if(nullptr == obj_->*m_modelPtr)
                {
                    const_cast<DataType_ *>(obj_)->*m_modelPtr
                            = (const_cast<DataType_ *>(obj_)->*m_createFuncPtr)();
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
                    const_cast<DataType_ *>(obj1_)->*m_modelPtr
                            = (const_cast<DataType_ *>(obj1_)->*m_createFuncPtr)();
                }
                if(nullptr == obj2_->*m_modelPtr)
                {
                    const_cast<DataType_ *>(obj2_)->*m_modelPtr
                            = (const_cast<DataType_ *>(obj2_)->*m_createFuncPtr)();
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
        class LayoutSpecialField : public ILayoutItemBase<DataType_>
        {
        public:
            using itemMemberPtr = ItemType_ DataType_::*;

            LayoutSpecialField(int index_,
                             itemMemberPtr itemPtr,
                             SpecialFieldEn type_
                             )
                : ILayoutItemBase<DataType_>(index_, QString(), QString(), type_),
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
private:
    using LayoutItemBase = layout::Private::ILayoutItemBase<DataType_>;

public:
    LayoutBase() = default;
    ~LayoutBase()
    {
        clearData();
    }
    LayoutBase(const LayoutBase &) = delete;
    LayoutBase(const LayoutBase &&) = delete;
    LayoutBase &operator = (const LayoutBase &) = delete;
    LayoutBase &operator = (const LayoutBase &&) = delete;

    template<typename ItemType_>
    void addField(const QString &jsonName_,
                  const QString &qmlName_,
                  typename layout::Private::LayoutField<DataType_,ItemType_>::getFuncPtr getFunc_,
                  typename layout::Private::LayoutField<DataType_,ItemType_>::setFuncPtr setFunc_)
    {
        m_fields.push_back(
                    new layout::Private::LayoutField<DataType_,ItemType_>(
                        m_lastQMLIndex, jsonName_, qmlName_, getFunc_, setFunc_)
                    );
        if(m_fields.back()->isQMLItem()) { m_lastQMLIndex++; }
    }

    template<typename ModelType_>
    void addModel(const QString &qmlName_,
                  typename layout::Private::LayoutModelField<DataType_,ModelType_>::ModelTypeFieldPtr varPtr_,
                  typename layout::Private::LayoutModelField<DataType_,ModelType_>::createModelFuncPtr createFunc_)
    {
        m_fields.push_back(
                    new layout::Private::LayoutModelField<DataType_,ModelType_>(
                        m_lastQMLIndex, qmlName_, varPtr_, createFunc_)
                    );
        if(m_fields.back()->isQMLItem()) { m_lastQMLIndex++; }
    }

    template<typename ItemType_>
    void addSpecial(layout::SpecialFieldEn type_,
                  typename layout::Private::LayoutSpecialField<DataType_,ItemType_>::itemMemberPtr varPtr_
                  )
    {
        m_fields.push_back(
                    new layout::Private::LayoutSpecialField<DataType_,ItemType_>(
                        m_lastQMLIndex, varPtr_, type_)
                    );
    }

    void initQMLModelRoleNames(QHash<int, QByteArray> &roleNames_) const
    {
        for(const layout::Private::ILayoutItem<DataType_> *layoutItem: qAsConst(m_fields))
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
        for(const layout::Private::ILayoutItem<DataType_> *item : qAsConst(m_fields))
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
        for(const layout::Private::ILayoutItem<DataType_> *item : qAsConst(m_fields))
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
        for(const layout::Private::ILayoutItem<DataType_> *item : qAsConst(m_fields))
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
        for(const layout::Private::ILayoutItemBase<DataType_> *item : qAsConst(m_fields))
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
        for(const layout::Private::ILayoutItemBase<DataType_> *item : qAsConst(m_fields))
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
        for(const layout::Private::ILayoutItem<DataType_> *item : qAsConst(m_fields))
        {
            if(item->isJsonItem())
            {
                fieldsInfo_.push_back({item->getJsonName(), item->getJsonType()});
            }
        }
    }

    QVariant getSpecialFieldValue(layout::SpecialFieldEn type_, const DataType_ *obj_) const
    {
        for(const layout::Private::ILayoutItemBase<DataType_> *item : qAsConst(m_fields))
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
        for(const layout::Private::ILayoutItemBase<DataType_> *item : qAsConst(m_fields))
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
        layout::Private::ILayoutItemBase<DataType_> *layoutItem = findItemByJsonName(fieldJsonName_);
        Q_ASSERT(nullptr != layoutItem); // field not founded
        if(nullptr == layoutItem) { return; }
        layoutItem->setFieldId(true);
    }

    bool copyQMLFields(const DataType_ *from_, DataType_ *to_) const
    {
        bool ret = false;
        for(const layout::Private::ILayoutItemBase<DataType_> *item : qAsConst(m_fields))
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

private:
    template<typename JsonType_>
    bool setJsonValuesTempl(DataType_ *obj_, const JsonType_ &jsonObj_) const
    {
        bool ret = false;
        for(const layout::Private::ILayoutItem<DataType_> *item : qAsConst(m_fields))
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

    template<typename LayoutType_>
    static auto *findItemByJsonNameImpl(
            LayoutType_ &layout_,
            const QString &fieldJsonName_
            )
    {
        auto fit = std::find_if(
                    std::begin(layout_.m_fields),
                    std::end(layout_.m_fields),
                    [&fieldJsonName_](const layout::Private::ILayoutItem<DataType_> *item_)->bool
        {
            return nullptr != item_ && item_->getJsonName() == fieldJsonName_;
        });
        if(std::end(layout_.m_fields) == fit)
        {
            using PtrType = decltype (*fit);
            return static_cast<PtrType>(nullptr);
        }
        return *fit;
    }

    const layout::Private::ILayoutItemBase<DataType_> *findItemByJsonName(const QString &fieldJsonName_) const
    {
        return findItemByJsonNameImpl(*this, fieldJsonName_);
    }

    layout::Private::ILayoutItemBase<DataType_> *findItemByJsonName(const QString &fieldJsonName_)
    {
        return findItemByJsonNameImpl(*this, fieldJsonName_);
    }

    template<typename LayoutType_>
    static auto *findItemByQMLNameImpl(
            LayoutType_ &layout_,
            const QString &fieldQMLName_
            )
    {
        auto fit = std::find_if(
                    std::begin(layout_.m_fields),
                    std::end(layout_.m_fields),
                    [&fieldQMLName_](const layout::Private::ILayoutItem<DataType_> *item_)->bool
        {
            return nullptr != item_ && item_->getQMLName() == fieldQMLName_;
        });
        if(std::end(layout_.m_fields) == fit)
        {
            using PtrType = decltype (*fit);
            return static_cast<PtrType>(nullptr);
        }
        return *fit;
    }

    const layout::Private::ILayoutItem<DataType_> *findItemByQMLName(const QString &fieldQMLName_) const
    {
        return findItemByQMLNameImpl(*this, fieldQMLName_);
    }

    layout::Private::ILayoutItem<DataType_> *findItemByQMLName(const QString &fieldQMLName_)
    {
        return findItemByQMLNameImpl(*this, fieldQMLName_);
    }

    template<typename LayoutType_>
    static auto *findItemByModelRoleImpl(
            LayoutType_ &layout_,
            int role_
            )
    {
        auto fit = std::find_if(
                    std::begin(layout_.m_fields),
                    std::end(layout_.m_fields),
                    [&role_](const layout::Private::ILayoutItem<DataType_> *item_)->bool
        {
            return nullptr != item_
                    && item_->isQMLItem()
                    && item_->getModelIndex() + Qt::UserRole == role_
                    ;
        });
        if(std::end(layout_.m_fields) == fit)
        {
            using PtrType = decltype (*fit);
            return static_cast<PtrType>(nullptr);
        }
        return *fit;
    }

    const layout::Private::ILayoutItem<DataType_> *findItemByModelRole(int role_) const
    {
        return findItemByModelRoleImpl(*this, role_);
    }

    layout::Private::ILayoutItem<DataType_> *findItemByModelRole(int role_)
    {
        return findItemByModelRoleImpl(*this, role_);
    }

    template<typename JsonType_>
    void setJsonValues(DataType_ *obj_, const JsonType_ &jsonObj_)
    {
        for(const layout::Private::ILayoutItem<DataType_> *item : qAsConst(m_fields))
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
    QVector<layout::Private::ILayoutItemBase<DataType_> *> m_fields;
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
