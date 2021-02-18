#ifndef MODELHELPERS_H
#define MODELHELPERS_H


#include "../MastactvaBase/IModel.h"


template<class ModelDataType_, class ModelType_>
class SortModelAfterChangeImpl : public IListModelChangeNotify
{
public:
    virtual void listLoadedVF() override
    {
        sortItems();
    }

    virtual void itemAddedVF() override
    {
        sortItems();
    }

    virtual void itemSetVF() override
    {
        sortItems();
    }

    virtual void itemDeletedVF() override
    {
        sortItems();
    }

    virtual void errorVF(int errorCode_,
                         const QString &errorCodeStr_,
                         const QJsonDocument &reply_) override
    {
        Q_UNUSED(errorCode_);
        Q_UNUSED(errorCodeStr_);
        Q_UNUSED(reply_);
    }

    virtual void objectLoadedVF() override
    {
        Q_ASSERT(false); // should not be called
    }

protected:
    virtual bool compareModelItems(const ModelDataType_ &i1_, const ModelDataType_ &i2_) const = 0;

    void init()
    {
        static_cast<ModelType_*>(this)->setModelChangeNotify(this);
    }

private:
    void sortItems()
    {
        static_cast<ModelType_*>(this)->sortIf(
                    [this] (const ModelDataType_ &i1_, const ModelDataType_ &i2_) -> bool
            { return compareItems(i1_, i2_); },
            true, false);
    }
};


#endif // MODELHELPERS_H
