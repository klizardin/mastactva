#ifndef MODELHELPERS_H
#define MODELHELPERS_H


#include <algorithm>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


template<class ModelDataType_, class ModelType_, class ModelDataObjectType_ = ModelDataType_>
class SortModelAfterChangeImpl : public IListModelChangeNotify
{
    /*
     * ModelType_ should implement:
     *
     * bool compareModelItems(const ModelDataType_ *i1_, const ModelDataType_ *i2_) const;
     * void currentIndexAfterSortChanged();
     *
     * */

public:
    virtual void listLoadedVF() override
    {
        if(sortItems())
        {
            ModelType_* model = static_cast<ModelType_*>(this);
            model->currentIndexAfterSortChanged();
        }
    }

    virtual void itemAddedVF() override
    {
        if(sortItems())
        {
            ModelType_* model = static_cast<ModelType_*>(this);
            model->currentIndexAfterSortChanged();
        }
    }

    virtual void itemSetVF() override
    {
        if(sortItems())
        {
            ModelType_* model = static_cast<ModelType_*>(this);
            model->currentIndexAfterSortChanged();
        }
    }

    virtual void itemDeletedVF() override
    {
        if(sortItems())
        {
            ModelType_* model = static_cast<ModelType_*>(this);
            model->currentIndexAfterSortChanged();
        }
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
    void init()
    {
        ModelType_* model = static_cast<ModelType_*>(this);
        ListModelBaseOfData<ModelDataType_, ModelType_, ModelDataObjectType_>* listModel
                = static_cast<ListModelBaseOfData<ModelDataType_, ModelType_, ModelDataObjectType_>*>(model);
        listModel->setModelChangeNotify(this);
    }

private:
    bool sortItems()
    {
        ModelType_* model = static_cast<ModelType_*>(this);
        ListModelBaseOfData<ModelDataType_, ModelType_, ModelDataObjectType_>* listModel
                = static_cast<ListModelBaseOfData<ModelDataType_, ModelType_, ModelDataObjectType_>*>(model);
        return listModel->sortIf(
                    [&model](const ModelDataType_ *i1_, const ModelDataType_ *i2_)->bool
                    {
                        const ModelDataObjectType_ *i1 = static_cast<const ModelDataObjectType_ *>(i1_);
                        const ModelDataObjectType_ *i2 = static_cast<const ModelDataObjectType_ *>(i2_);
                        return model->compareModelItems(i1, i2);
                    },
                    true,false);
    }
};


#endif // MODELHELPERS_H
