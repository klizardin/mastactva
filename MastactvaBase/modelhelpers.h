/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MODELHELPERS_H
#define MODELHELPERS_H


#include <algorithm>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


template<class ModelDataType_, class ModelType_, class ModelBase_, class ModelDataObjectType_ = ModelDataType_>
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
    SortModelAfterChangeImpl(ModelType_ *model_)
        : m_model(model_)
    {
    }

    virtual void listLoadedVF() override
    {
        if(sortItems())
        {
            if(!m_model) { return; }
            m_model->currentIndexAfterSortChanged();
        }
    }

    virtual void itemAddedVF() override
    {
        if(sortItems())
        {
            if(!m_model) { return; }
            m_model->currentIndexAfterSortChanged();
        }
    }

    virtual void itemSetVF() override
    {
        if(sortItems())
        {
            if(!m_model) { return; }
            m_model->currentIndexAfterSortChanged();
        }
    }

    virtual void itemDeletedVF() override
    {
        if(sortItems())
        {
            if(!m_model) { return; }
            m_model->currentIndexAfterSortChanged();
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

    void init()
    {
        if(!m_model) { return; }
        ListModelBaseOfData<ModelDataType_, ModelType_, ModelDataObjectType_>* listModel
                = static_cast<ListModelBaseOfData<ModelDataType_, ModelType_, ModelDataObjectType_>*>(m_model);
        listModel->setModelChangeNotify(this);
    }

private:
    bool sortItems()
    {
        if(!m_model) { return false; }
        ListModelBaseOfData<ModelDataType_, ModelType_, ModelDataObjectType_>* listModel
                = static_cast<ListModelBaseOfData<ModelDataType_, ModelType_, ModelDataObjectType_>*>(m_model);
        return listModel->sortIf(
                    [this](const ModelDataObjectType_ *i1_, const ModelDataObjectType_ *i2_)->bool
                    {
                        return m_model->compareModelItems(i1_, i2_);
                    },
                    true,false);
    }

private:
    ModelType_ *m_model = nullptr;
};


#endif // MODELHELPERS_H
